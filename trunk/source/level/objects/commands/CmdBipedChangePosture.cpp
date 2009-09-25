/***
 * hesperus: CmdBipedChangePosture.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "CmdBipedChangePosture.h"

#include <source/level/collisions/Bounds.h>
#include <source/level/collisions/BoundsManager.h>
#include <source/level/objects/components/ICmpBounds.h>
#include <source/level/objects/components/ICmpMeshMovement.h>
#include <source/level/objects/components/ICmpPosition.h>
#include <source/level/trees/OnionTree.h>
#include <source/level/trees/TreeUtil.h>

namespace hesp {

//#################### CONSTRUCTORS ####################
CmdBipedChangePosture::CmdBipedChangePosture(const ObjectID& objectID)
:	m_objectID(objectID)
{}

//#################### PUBLIC METHODS ####################
void CmdBipedChangePosture::execute(const ObjectManager_Ptr& objectManager, const std::vector<CollisionPolygon_Ptr>& polygons, const OnionTree_CPtr& tree,
									const std::vector<NavDataset_Ptr>& navDatasets, int milliseconds)
{
	// FIXME: Crouching is currently a "jolt" from one pose to another. It should really be a smooth transition.

	ICmpBounds_Ptr cmpBounds = objectManager->get_component(m_objectID, cmpBounds);				assert(cmpBounds != NULL);
	ICmpMeshMovement_Ptr cmpMovement = objectManager->get_component(m_objectID, cmpMovement);	assert(cmpMovement != NULL);
	ICmpPosition_Ptr cmpPosition = objectManager->get_component(m_objectID, cmpPosition);		assert(cmpPosition != NULL);

	// Check that we're not currently traversing a nav link.
	if(cmpMovement->cur_traversal()) return;

	Vector3d source = cmpPosition->position();

	const std::string& curPosture = cmpBounds->posture();
	std::string newPosture = curPosture == "stand" ? "crouch" : "stand";
	BoundsManager_CPtr boundsManager = objectManager->bounds_manager();
	int curMapIndex = boundsManager->lookup_bounds_index(cmpBounds->bounds_group(), curPosture);
	int newMapIndex = boundsManager->lookup_bounds_index(cmpBounds->bounds_group(), newPosture);
	const Bounds_CPtr& curBounds = boundsManager->bounds(curMapIndex);
	const Bounds_CPtr& newBounds = boundsManager->bounds(newMapIndex);

	double deltaZ = (newBounds->height() - curBounds->height()) / 2;

	Vector3d dest = source + Vector3d(0,0,deltaZ);

	// Check that changing pose won't put us in a wall.
	int destLeafIndex = TreeUtil::find_leaf_index(dest, tree);
	const OnionLeaf *destLeaf = tree->leaf(destLeafIndex);
	if(destLeaf->is_solid(newMapIndex)) return;

	// If the posture change is ok, set the new posture and update the object position to reflect the centre of the new bounds.
	cmpBounds->set_posture(newPosture);
	cmpPosition->set_position(dest);
	cmpMovement->set_cur_nav_poly_index(-1);
}

}
