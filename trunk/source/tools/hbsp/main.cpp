/***
 * hbsp: main.cpp
 * Copyright Stuart Golodetz, 2008. All rights reserved.
 ***/

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
using boost::bad_lexical_cast;
using boost::lexical_cast;

#include <source/io/files/GeometryFile.h>
#include <source/io/files/TreeFile.h>
#include <source/level/trees/BSPCompiler.h>
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
	std::cout << "Usage: hbsp {-r|-c} <input geometry> {<input hints>|nohints} <output tree> [-w<number>]" << std::endl;
	exit(EXIT_FAILURE);
}

template <typename Poly>
void run_compiler(const std::string& inputFilename, const std::string& outputFilename, const double weight)
{
	typedef shared_ptr<Poly> Poly_Ptr;
	typedef std::vector<Poly_Ptr> PolyVector;

	// Load the input polygons from disk.
	PolyVector polygons;
	GeometryFile::load(inputFilename, polygons);

	// Build the BSP tree.
	BSPTree_Ptr tree = BSPCompiler::build_tree(polygons, weight);

	// Save the polygons and the BSP tree to the output file.
	TreeFile::save(outputFilename, polygons, tree);
}

template <typename Poly>
void run_compiler_ex(const std::string& inputGeometryFilename, const std::string& hintGeometryFilename, const std::string& outputTreeFilename, double weight)
{
	typedef shared_ptr<Poly> Poly_Ptr;
	typedef std::vector<Poly_Ptr> PolyVector;

	// Load the input polygons from disk.
	PolyVector polygons;
	GeometryFile::load(inputGeometryFilename, polygons);

	// Load the hint polygons from disk.
	PolyVector hintPolygons;
	if(hintGeometryFilename != "nohints") GeometryFile::load(hintGeometryFilename, hintPolygons);

	// Build the BSP tree.
	BSPCompiler<Poly> compiler(polygons, hintPolygons, weight);
	compiler.build_tree();

	// Save the polygons and the BSP tree to the output file.
	TreeFile::save(outputTreeFilename, compiler.polygons(), compiler.tree());
}

int main(int argc, char *argv[])
try
{
	if(argc != 5 && argc != 6) quit_with_usage();

	const std::vector<std::string> args(argv, argv+argc);

	std::string inputGeometryFilename = args[2];
	std::string hintGeometryFilename = args[3];
	std::string outputTreeFilename = args[4];

	double weight = 4;
	if(argc == 6)
	{
		if(args[5].substr(0,2) != "-w") quit_with_usage();

		try							{ weight = lexical_cast<double,std::string>(args[5].substr(2)); }
		catch(bad_lexical_cast&)	{ quit_with_usage(); }
	}

	if(args[1] == "-r") run_compiler_ex<TexturedPolygon>(inputGeometryFilename, hintGeometryFilename, outputTreeFilename, weight);
	else if(args[1] == "-c") run_compiler_ex<CollisionPolygon>(inputGeometryFilename, hintGeometryFilename, outputTreeFilename, weight);
	else quit_with_usage();

	return 0;
}
catch(Exception& e) { quit_with_error(e.cause()); }
