/***
 * hesperus: CmpScriptedActivatable.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "CmpScriptedActivatable.h"

// TEMPORARY
#include <iostream>

#include <source/level/objects/base/ObjectManager.h>

namespace hesp {

//#################### CONSTRUCTORS ####################
CmpScriptedActivatable::CmpScriptedActivatable(const std::string& scriptName)
:	m_scriptName(scriptName)
{
	// TODO
}

//#################### STATIC FACTORY METHODS ####################
IComponent_Ptr CmpScriptedActivatable::load(const Properties& properties)
{
	return IComponent_Ptr(new CmpScriptedActivatable(properties.get_actual<std::string>("Script")));
}

//#################### PUBLIC METHODS ####################
void CmpScriptedActivatable::activated_by(const ObjectID& activator)
{
	// TEMPORARY
	std::cout << "Object " << m_objectID.to_string() << " activated by object " << activator.to_string() << std::endl;
}

std::pair<std::string,Properties> CmpScriptedActivatable::save() const
{
	Properties properties;
	properties.set_actual("Script", m_scriptName);
	return std::make_pair("ScriptedActivatable", properties);
}

}
