/***
 * hesperus: EntitiesFile.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "EntitiesFile.h"

#include <fstream>

namespace bf = boost::filesystem;

#include <source/exceptions/Exception.h>
#include <source/io/sections/EntitiesSection.h>

namespace hesp {

//#################### LOADING METHODS ####################
/**
Loads a set of entities from the specified entities file.

@param filename		The name of the file from which to load the entities
@param settingsDir	The location of the directory containing the project settings files (e.g. the entity definitions file)
@return				An EntityManager containing the loaded entities
*/
EntityManager_Ptr EntitiesFile::load(const std::string& filename, const bf::path& settingsDir)
{
	std::ifstream is(filename.c_str());
	if(is.fail()) throw Exception("Could not open " + filename + " for reading");
	return EntitiesSection::load(is, settingsDir);
}

//#################### SAVING METHODS ####################
/**
Saves a set of entities to the specified entities file.

@param filename			The name of the file to which to save the entities
@param entityManager	An EntityManager containing the entities to save
*/
void EntitiesFile::save(const std::string& filename, const EntityManager_Ptr& entityManager)
{
	std::ofstream os(filename.c_str());
	if(os.fail()) throw Exception("Could not open " + filename + " for writing");
	EntitiesSection::save(os, entityManager);
}

}