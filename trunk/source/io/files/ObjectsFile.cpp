/***
 * hesperus: ObjectsFile.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "ObjectsFile.h"

#include <fstream>

#include <source/exceptions/Exception.h>
#include <source/io/sections/ModelNamesSection.h>
#include <source/io/sections/ObjectsSection.h>
#include <source/level/objects/base/ObjectManager.h>

namespace hesp {

//#################### LOADING METHODS ####################
ObjectManager_Ptr ObjectsFile::load(const std::string& filename, const BoundsManager_CPtr& boundsManager,
									const ComponentPropertyTypeMap& componentPropertyTypes,
									const std::map<std::string,ObjectSpecification>& archetypes)
{
	std::ifstream is(filename.c_str());
	if(is.fail()) throw Exception("Could not open " + filename + " for reading");
	ModelManager_Ptr modelManager = ModelNamesSection::load(is);
	return ObjectsSection::load(is, modelManager, boundsManager, componentPropertyTypes, archetypes);
}

//#################### SAVING METHODS ####################
void ObjectsFile::save(const std::string& filename, const ObjectManager_Ptr& objectManager)
{
	std::ofstream os(filename.c_str());
	if(os.fail()) throw Exception("Could not open " + filename + " for writing");
	ModelNamesSection::save(os, objectManager->model_manager());
	ObjectsSection::save(os, objectManager);
}

}
