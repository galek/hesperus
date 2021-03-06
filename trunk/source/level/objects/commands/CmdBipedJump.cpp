/***
 * hesperus: CmdBipedJump.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "CmdBipedJump.h"

#include <source/level/bounds/BoundsManager.h>
#include <source/level/nav/NavDataset.h>
#include <source/level/nav/NavManager.h>
#include <source/level/nav/NavMesh.h>
#include <source/level/objects/components/ICmpMovement.h>
#include <source/level/objects/components/ICmpSimulation.h>

namespace hesp {

//#################### CONSTRUCTORS ####################
CmdBipedJump::CmdBipedJump(const ObjectID& objectID, const Vector3d& dir)
:	m_objectID(objectID), m_dir(dir)
{}

//#################### PUBLIC METHODS ####################
void CmdBipedJump::execute(const ObjectManager_Ptr& objectManager, const std::vector<CollisionPolygon_Ptr>& polygons, const OnionTree_CPtr& tree,
						   const NavManager_CPtr& navManager, int milliseconds)
{
	ICmpMovement_Ptr cmpMovement = objectManager->get_component(m_objectID, cmpMovement);			assert(cmpMovement != NULL);
	ICmpSimulation_Ptr cmpSimulation = objectManager->get_component(m_objectID, cmpSimulation);		assert(cmpSimulation != NULL);

	int mapIndex = objectManager->bounds_manager()->lookup_bounds_index(cmpSimulation->bounds_group(), cmpSimulation->posture());
	NavMesh_CPtr navMesh = navManager->dataset(mapIndex)->nav_mesh();

	if(cmpMovement->attempt_navmesh_acquisition(polygons, tree, navMesh))
	{
		// FIXME: The jump strength should eventually be a property of the entity.
		const double JUMP_STRENGTH = 3;		// force of jump in Newtons
		Vector3d velocity = m_dir / JUMP_STRENGTH + Vector3d(0,0,1) * JUMP_STRENGTH;
		cmpSimulation->set_velocity(velocity);
	}
}

}
