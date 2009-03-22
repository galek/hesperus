/***
 * hcollate: main.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
namespace bf = boost::filesystem;
using boost::bad_lexical_cast;
using boost::lexical_cast;

#include <source/exceptions/Exception.h>
#include <source/images/BitmapLoader.h>
#include <source/io/EntitiesFile.h>
#include <source/io/LevelFile.h>
#include <source/io/LitTreeFile.h>
#include <source/io/NavFile.h>
#include <source/io/OnionPortalsFile.h>
#include <source/io/OnionTreeFile.h>
#include <source/io/PortalsFile.h>
#include <source/io/TreeFile.h>
#include <source/io/VisFile.h>
#include <source/io/util/DirectoryFinder.h>
#include <source/util/PolygonTypes.h>
using namespace hesp;

//#################### FUNCTIONS ####################
void quit_with_error(const std::string& error)
{
	std::cout << "Error: " << error << std::endl;
	exit(EXIT_FAILURE);
}

void quit_with_usage()
{
	std::cout << "Usage: hcollate {+L <input lit tree> | -L <input tree>} <input portals> <input vis> <input onion tree> <input onion portals> <input nav data> <input entities> <output filename>" << std::endl;
	exit(EXIT_FAILURE);
}

void collate_lit(const std::string& treeFilename, const std::string& portalsFilename, const std::string& visFilename,
				 const std::string& onionTreeFilename, const std::string& onionPortalsFilename,
				 const std::string& navFilename, const std::string& entitiesFilename, const std::string& outputFilename)
try
{
	// Load the lit polygons, tree and lightmap prefix.
	typedef std::vector<TexturedLitPolygon_Ptr> TexLitPolyVector;
	TexLitPolyVector polygons;
	BSPTree_Ptr tree;
	std::string lightmapPrefix;
	LitTreeFile::load(treeFilename, polygons, tree, lightmapPrefix);

	// Load the portals.
	int emptyLeafCount;
	std::vector<Portal_Ptr> portals;
	PortalsFile::load(portalsFilename, emptyLeafCount, portals);

	// Load the vis table.
	LeafVisTable_Ptr leafVis = VisFile::load(visFilename);

	// Load the lightmaps.
	int polyCount = static_cast<int>(polygons.size());
	std::vector<Image24_Ptr> lightmaps(polyCount);
	for(int i=0; i<polyCount; ++i)
	{
		std::string filename = lightmapPrefix + lexical_cast<std::string,int>(i) + ".bmp";
		lightmaps[i] = BitmapLoader::load_image24(filename);
	}

	// Load the onion tree.
	typedef std::vector<CollisionPolygon_Ptr> ColPolyVector;
	ColPolyVector onionPolygons;
	OnionTree_Ptr onionTree;
	OnionTreeFile::load(onionTreeFilename, onionPolygons, onionTree);

	// Load the onion portals.
	std::vector<OnionPortal_Ptr> onionPortals = OnionPortalsFile::load(onionPortalsFilename);

	// Load the navigation data.
	std::vector<NavDataset_Ptr> navDatasets = NavFile::load(navFilename);

	// Load the entities.
	bf::path settingsDir = determine_settings_directory(determine_base_directory_from_tool());
	EntityManager_Ptr entityManager = EntitiesFile::load(entitiesFilename, settingsDir);

	// Write everything to the output file.
	LevelFile::save_lit(outputFilename,
						polygons, tree,
						portals,
						leafVis,
						lightmaps,
						onionPolygons, onionTree,
						onionPortals,
						navDatasets,
						entityManager);
}
catch(Exception& e) { quit_with_error(e.cause()); }

void collate_unlit(const std::string& treeFilename, const std::string& portalsFilename, const std::string& visFilename,
				   const std::string& onionTreeFilename, const std::string& onionPortalsFilename,
				   const std::string& navFilename, const std::string& entitiesFilename, const std::string& outputFilename)
try
{
	// Load the unlit polygons and tree.
	typedef std::vector<TexturedPolygon_Ptr> TexPolyVector;
	TexPolyVector polygons;
	BSPTree_Ptr tree;
	TreeFile::load(treeFilename, polygons, tree);

	// Load the portals.
	int emptyLeafCount;
	std::vector<Portal_Ptr> portals;
	PortalsFile::load(portalsFilename, emptyLeafCount, portals);

	// Load the vis table.
	LeafVisTable_Ptr leafVis = VisFile::load(visFilename);

	// Load the onion tree.
	typedef std::vector<CollisionPolygon_Ptr> ColPolyVector;
	ColPolyVector onionPolygons;
	OnionTree_Ptr onionTree;
	OnionTreeFile::load(onionTreeFilename, onionPolygons, onionTree);

	// Load the onion portals.
	std::vector<OnionPortal_Ptr> onionPortals = OnionPortalsFile::load(onionPortalsFilename);

	// Load the navigation data.
	std::vector<NavDataset_Ptr> navDatasets = NavFile::load(navFilename);

	// Load the entities.
	bf::path settingsDir = determine_settings_directory(determine_base_directory_from_tool());
	EntityManager_Ptr entityManager = EntitiesFile::load(entitiesFilename, settingsDir);

	// Write everything to the output file.
	LevelFile::save_unlit(outputFilename,
						  polygons, tree,
						  portals,
						  leafVis,
						  onionPolygons, onionTree,
						  onionPortals,
						  navDatasets,
						  entityManager);
}
catch(Exception& e) { quit_with_error(e.cause()); }

int main(int argc, char *argv[])
{
	if(argc != 10) quit_with_usage();
	std::vector<std::string> args(argv, argv + argc);

	if(args[1] == "+L") collate_lit(args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9]);
	else if(args[1] == "-L") collate_unlit(args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9]);
	else quit_with_usage();

	return 0;
}
