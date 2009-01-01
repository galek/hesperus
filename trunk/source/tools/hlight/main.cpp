/***
 * hlight: main.cpp
 * Copyright Stuart Golodetz, 2008. All rights reserved.
 ***/

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
using boost::bad_lexical_cast;
using boost::lexical_cast;

#include <source/io/FileUtil.h>
#include <source/level/lighting/LightmapGenerator.h>
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
	std::cout << "Usage: hlight <input tree> <input lights> <input vis> <output filename>" << std::endl;
	exit(EXIT_FAILURE);
}

void run_generator(const std::string& treeFilename, const std::string& lightsFilename, const std::string& visFilename, const std::string& outputFilename)
{
	// Read in the polygons and tree.
	std::vector<TexturedPolygon_Ptr> polygons;
	BSPTree_Ptr tree;
	try
	{
		load_tree_file(treeFilename, polygons, tree);
	}
	catch(Exception& e) { quit_with_error(e.cause()); }
}

int main(int argc, char *argv[])
{
	if(argc != 5) quit_with_usage();
	std::vector<std::string> args(argv, argv + argc);
	run_generator(args[1], args[2], args[3], args[4]);

#if 0
	std::vector<TexturedVector3d> vertices;
	vertices.push_back(TexturedVector3d(2,3,5,0,0));
	vertices.push_back(TexturedVector3d(2,1,5,0,1));
	vertices.push_back(TexturedVector3d(10,3,6,1,0));
	TexturedPolygon poly(vertices, "TEST");
	std::vector<TexCoords> vertexLightmapCoords;
	LightmapGrid grid(poly, vertexLightmapCoords);
#else
	std::vector<TexturedPolygon_Ptr> polys;
	{
		// Projected onto x-y plane.
		std::vector<TexturedVector3d> vertices;
		vertices.push_back(TexturedVector3d(2,3,5,0,0));
		vertices.push_back(TexturedVector3d(2,1,5,0,1));
		vertices.push_back(TexturedVector3d(10,3,6,1,0));
		polys.push_back(TexturedPolygon_Ptr(new TexturedPolygon(vertices, "TEST")));
	}
	{
		// Projected onto x-z plane.
		std::vector<TexturedVector3d> vertices;
		vertices.push_back(TexturedVector3d(1,5,1,0,1));
		vertices.push_back(TexturedVector3d(3,5,10,1,0));
		vertices.push_back(TexturedVector3d(3,6,1,1,1));
		polys.push_back(TexturedPolygon_Ptr(new TexturedPolygon(vertices, "TEST")));
	}
	{
		// Projected onto y-z plane.
		std::vector<TexturedVector3d> vertices;
		vertices.push_back(TexturedVector3d(-2,0,0,0,0));
		vertices.push_back(TexturedVector3d(-2,10,-10,1,1));
		vertices.push_back(TexturedVector3d(-3,0,-10,0,1));
		polys.push_back(TexturedPolygon_Ptr(new TexturedPolygon(vertices, "TEST")));
	}
	LightmapGenerator lg(polys, std::vector<Light>(), BSPTree_Ptr(), LeafVisTable_Ptr());
	lg.generate_lightmaps();
#endif

	return 0;
}
