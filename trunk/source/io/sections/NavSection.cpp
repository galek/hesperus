/***
 * hesperus: NavSection.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "NavSection.h"

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
using boost::bad_lexical_cast;
using boost::lexical_cast;

#include <source/exceptions/Exception.h>
#include <source/io/util/LineIO.h>
#include <source/io/util/NavLinkFactory.h>
#include <source/level/nav/AdjacencyList.h>
#include <source/level/nav/NavDataset.h>
#include <source/level/nav/NavLink.h>
#include <source/level/nav/NavMesh.h>
#include <source/level/nav/NavPolygon.h>
#include <source/level/nav/PathTable.h>

namespace hesp {

//#################### LOADING METHODS ####################
/**
Loads an array of navigation datasets from the specified std::istream.

@param is	The std::istream
@return		The navigation datasets
*/
std::vector<NavDataset_Ptr> NavSection::load(std::istream& is)
{
	std::vector<NavDataset_Ptr> datasets;

	LineIO::read_checked_line(is, "Nav");
	LineIO::read_checked_line(is, "{");

	std::string line;
	LineIO::read_line(is, line, "navigation dataset count");
	int datasetCount;
	try							{ datasetCount = lexical_cast<int,std::string>(line); }
	catch(bad_lexical_cast&)	{ throw Exception("The navigation dataset count was not an integer"); }

	for(int i=0; i<datasetCount; ++i)
	{
		LineIO::read_checked_line(is, "Dataset");
		LineIO::read_checked_line(is, "{");

		NavMesh_Ptr navMesh = read_navmesh(is);
		AdjacencyList_Ptr adjList = read_adjacency_list(is);
		PathTable_Ptr pathTable = read_path_table(is);

		datasets.push_back(NavDataset_Ptr(new NavDataset(adjList, navMesh, pathTable)));

		LineIO::read_checked_line(is, "}");
	}

	LineIO::read_checked_line(is, "}");

	return datasets;
}

//#################### SAVING METHODS ####################
/**
Saves an array of navigation datasets to the specified std::ostream.

@param os			The std::ostream
@param datasets		The navigation datasets
*/
void NavSection::save(std::ostream& os, const std::vector<NavDataset_Ptr>& datasets)
{
	os << "Nav\n";
	os << "{\n";

	// Output the datasets sequentially.
	int datasetCount = static_cast<int>(datasets.size());
	os << datasetCount << '\n';
	for(int i=0; i<datasetCount; ++i)
	{
		os << "Dataset\n";
		os << "{\n";

		write_navmesh(os, datasets[i]->nav_mesh());
		write_adjacency_list(os, datasets[i]->adjacency_list());
		write_path_table(os, datasets[i]->path_table());

		os << "}\n";
	}

	os << "}\n";
}

//#################### LOADING SUPPORT METHODS ####################
/**
Reads an adjacency list from the specified std::istream.
*/
AdjacencyList_Ptr NavSection::read_adjacency_list(std::istream& is)
{
	LineIO::read_checked_line(is, "AdjacencyList");
	LineIO::read_checked_line(is, "{");

	std::string line;
	LineIO::read_line(is, line, "adjacency list size");
	int size;
	try							{ size = lexical_cast<int,std::string>(line); }
	catch(bad_lexical_cast&)	{ throw Exception("The adjacency list size was not an integer"); }

	AdjacencyList_Ptr adjList(new AdjacencyList(size));

	for(int i=0; i<size; ++i)
	{
		LineIO::read_line(is, line, "adjacency list entry");

		typedef boost::char_separator<char> sep;
		typedef boost::tokenizer<sep> tokenizer;
		tokenizer tok(line.begin(), line.end(), sep(" "));
		std::vector<std::string> tokens(tok.begin(), tok.end());

		// Check the adjacency list entry for validity.
		int tokenCount = static_cast<int>(tokens.size());
		if(tokenCount == 0) throw Exception("Missing adjacency list entry " + lexical_cast<std::string,int>(i));
		if(tokenCount % 4 != 1) throw Exception("Bad adjacency list entry " + lexical_cast<std::string,int>(i));

		try
		{
			int checkI = lexical_cast<int,std::string>(tokens[0]);
			if(checkI != i) throw Exception("Bad adjacency list entry " + lexical_cast<std::string,int>(i));
		}
		catch(bad_lexical_cast&)
		{
			throw Exception("The first token in the adjacency list entry was not an integer");
		}

		// Parse the edges.
		for(int j=1; j<tokenCount; j += 4)
		{
			if(tokens[j] != "(") throw Exception("Expected ( to start adjacency list entry edge");
			if(tokens[j+3] != ")") throw Exception("Expected ) to finish adjacency list entry edge");

			try
			{
				int toNode = lexical_cast<int,std::string>(tokens[j+1]);
				float length = lexical_cast<float,std::string>(tokens[j+2]);
				adjList->add_edge(i, AdjacencyList::Edge(toNode, length));
			}
			catch(bad_lexical_cast&)
			{
				throw Exception("Bad edge in adjacency list entry " + lexical_cast<std::string,int>(i));
			}
		}
	}

	LineIO::read_checked_line(is, "}");

	return adjList;
}

/**
Reads a navigation mesh from the specified std::istream.
*/
NavMesh_Ptr NavSection::read_navmesh(std::istream& is)
{
	LineIO::read_checked_line(is, "Mesh");
	LineIO::read_checked_line(is, "{");

	// Read in the nav links.
	LineIO::read_checked_line(is, "Links");
	LineIO::read_checked_line(is, "{");

	NavLinkFactory navLinkFactory;

	std::vector<NavLink_Ptr> navLinks;

	std::string line;
	for(;;)
	{
		std::getline(is, line);
		if(line == "}") break;
		else navLinks.push_back(navLinkFactory.construct_navlink(line));
	}

	// Read in the nav polygons.
	LineIO::read_checked_line(is, "Polygons");
	LineIO::read_checked_line(is, "{");

	std::vector<NavPolygon_Ptr> navPolygons;

	for(;;)
	{
		std::getline(is, line);
		if(line == "}") break;

		std::stringstream ss;
		ss << line;

		ss >> std::skipws;

		int index, colIndex;
		std::string dummy;
		ss >> index >> colIndex >> dummy;

		if(dummy != "[") throw Exception("Expected [ to start in links in nav polygon");

		NavPolygon_Ptr poly(new NavPolygon(colIndex));

		// Read the in links.
		for(;;)
		{
			ss >> dummy;
			if(dummy == "]") break;

			try
			{
				int inLink = lexical_cast<int,std::string>(dummy);
				poly->add_in_link(inLink);
			}
			catch(bad_lexical_cast&)
			{
				throw Exception("Bad in link for nav polygon " + lexical_cast<std::string,int>(index));
			}
		}

		// Read the out links.
		ss >> dummy;
		if(dummy != "[") throw Exception("Expected [ to start out links in nav polygon");

		for(;;)
		{
			ss >> dummy;
			if(dummy == "]") break;

			try
			{
				int outLink = lexical_cast<int,std::string>(dummy);
				poly->add_out_link(outLink);
			}
			catch(bad_lexical_cast&)
			{
				throw Exception("Bad out link for nav polygon " + lexical_cast<std::string,int>(index));
			}
		}

		navPolygons.push_back(poly);
	}

	LineIO::read_checked_line(is, "}");

	return NavMesh_Ptr(new NavMesh(navPolygons, navLinks));
}

/**
Reads a (binary format) path table from the specified std::istream.
*/
PathTable_Ptr NavSection::read_path_table(std::istream& is)
{
	LineIO::read_checked_line(is, "PathTable");
	LineIO::read_checked_line(is, "{");

	std::string line;
	LineIO::read_line(is, line, "path table size");
	int size;
	try							{ size = lexical_cast<int,std::string>(line); }
	catch(bad_lexical_cast&)	{ throw Exception("The path table size was not an integer"); }

	PathTable_Ptr pathTable(new PathTable(size));

	for(int i=0; i<size; ++i)
		for(int j=0; j<size; ++j)
		{
			// TODO: There may be endian issues with this if we ever port to another platform.
			is.read(reinterpret_cast<char*>(&pathTable->next_node(i,j)), sizeof(int));
			is.read(reinterpret_cast<char*>(&pathTable->cost(i,j)), sizeof(float));
		}

	if(is.get() != '\n') throw Exception("Expected newline after path table");

	LineIO::read_checked_line(is, "}");

	return pathTable;
}

//#################### SAVING SUPPORT METHODS ####################
/**
Writes an adjacency list to the specified std::ostream.
*/
void NavSection::write_adjacency_list(std::ostream& os, const AdjacencyList_CPtr& adjList)
{
	os << "AdjacencyList\n";
	os << "{\n";

	int size = adjList->size();
	os << size << '\n';
	for(int i=0; i<size; ++i)
	{
		os << i << ' ';
		const std::list<AdjacencyList::Edge>& adjEdges = adjList->adjacent_edges(i);
		for(std::list<AdjacencyList::Edge>::const_iterator jt=adjEdges.begin(), jend=adjEdges.end(); jt!=jend;)
		{
			os << "( " << jt->to_node() << ' ' << jt->length() << " )";
			++jt;
			if(jt != jend) os << ' ';
		}
		os << '\n';
	}

	os << "}\n";
}

/**
Writes a navigation mesh to the specified std::ostream.
*/
void NavSection::write_navmesh(std::ostream& os, const NavMesh_CPtr& mesh)
{
	os << "Mesh\n";
	os << "{\n";

	// Output the nav links.
	os << "Links\n";
	os << "{\n";
	const std::vector<NavLink_Ptr>& links = mesh->links();
	int linkCount = static_cast<int>(links.size());
	for(int i=0; i<linkCount; ++i)
	{
		links[i]->output(os);
		os << '\n';
	}
	os << "}\n";

	// Output the polygons.
	os << "Polygons\n";
	os << "{\n";
	const std::vector<NavPolygon_Ptr>& polygons = mesh->polygons();
	int polyCount = static_cast<int>(polygons.size());
	for(int i=0; i<polyCount; ++i)
	{
		os << i << ' ' << polygons[i]->collision_poly_index();

		// Output the in links for this polygon.
		os << " [ ";
		const std::vector<int>& inLinks = polygons[i]->in_links();
		size_t inLinkCount = inLinks.size();
		for(size_t j=0; j<inLinkCount; ++j) os << inLinks[j] << ' ';
		os << "]";

		// Output the out links for this polygon.
		os << " [ ";
		const std::vector<int>& outLinks = polygons[i]->out_links();
		size_t outLinkCount = outLinks.size();
		for(size_t j=0; j<outLinkCount; ++j) os << outLinks[j] << ' ';
		os << "]\n";

	}
	os << "}\n";

	os << "}\n";
}

/**
Writes a path table to the specified std::ostream in binary format.
*/
void NavSection::write_path_table(std::ostream& os, const PathTable_CPtr& pathTable)
{
	os << "PathTable\n";
	os << "{\n";

	int size = pathTable->size();
	os << size << '\n';

	for(int i=0; i<size; ++i)
		for(int j=0; j<size; ++j)
		{
			// TODO: There may be endian issues with this if we ever port to another platform.
			os.write(reinterpret_cast<const char*>(&pathTable->next_node(i,j)), sizeof(int));
			os.write(reinterpret_cast<const char*>(&pathTable->cost(i,j)), sizeof(float));
		}

	os << "\n}\n";
}

}
