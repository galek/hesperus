/***
 * hesperus: EntityManager.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "EntityManager.h"

#include <iostream>

#include <source/exceptions/Exception.h>

namespace hesp {

//#################### CONSTRUCTORS ####################
EntityManager::EntityManager(const std::vector<Entity_Ptr>& entities,
							 const std::vector<AABB3d>& aabbs,
							 const std::string& entDefFilename)
:	m_entities(entities), m_aabbs(aabbs), m_entDefFilename(entDefFilename)
{
	// Set the entity IDs and add the entities to the relevant arrays.
	int entityCount = static_cast<int>(m_entities.size());
	for(int i=0; i<entityCount; ++i)
	{
		entities[i]->set_id(i);
		if(entities[i]->physics_component()) m_simulables.push_back(entities[i]);
		if(entities[i]->visibility_component()) m_visibles.push_back(entities[i]);

		if(entities[i]->yoke_component())
		{
			const std::string& yokeType = entities[i]->yoke_component()->yoke_type();
			if(yokeType != "None") m_yokeables.push_back(entities[i]);
		}

		if(entities[i]->entity_type() == "Player")
		{
			if(!m_player) m_player = entities[i];
			else throw Exception("There can only be one player in a level");
		}
	}

	if(!m_player) throw Exception("The player must exist");
}

//#################### PUBLIC METHODS ####################
const std::vector<AABB3d>& EntityManager::aabbs() const
{
	return m_aabbs;
}

void EntityManager::save(std::ostream& os) const
{
	os << "Entities\n";
	os << "{\n";

	os << "DefinitionFile\n";
	os << "{\n";
	os << m_entDefFilename << '\n';
	os << "}\n";

	os << "Instances\n";
	os << "{\n";

	int entityCount = static_cast<int>(m_entities.size());
	os << entityCount << '\n';
	for(int i=0; i<entityCount; ++i)
	{
		m_entities[i]->save(os);
	}

	os << "}\n";

	os << "}\n";
}

Entity_Ptr EntityManager::player() const
{
	return m_player;
}

const std::vector<Entity_Ptr>& EntityManager::simulables() const
{
	return m_simulables;
}

const std::vector<Entity_Ptr>& EntityManager::visibles() const
{
	return m_visibles;
}

const std::vector<Entity_Ptr>& EntityManager::yokeables() const
{
	return m_yokeables;
}

}
