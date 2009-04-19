/***
 * hesperus: EntDefFile.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "EntDefFile.h"

#include <fstream>

#include <source/exceptions/Exception.h>
#include <source/io/sections/AABBsSection.h>
#include <source/io/util/LineIO.h>

namespace hesp {

//#################### LOADING METHODS ####################
/**
Loads an array of AABBs from the specified entity definition file.

@param filename	The name of the file from which to load the AABBs
@return			The AABBs
*/
std::vector<AABB3d> EntDefFile::load_aabbs_only(const std::string& filename)
{
	std::ifstream is(filename.c_str());
	if(is.fail()) throw Exception("Could not open " + filename + " for reading");

	LineIO::read_checked_line(is, "HENTDEF");
	return AABBsSection::load(is);
}

}
