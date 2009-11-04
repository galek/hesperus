/***
 * hesperus: CmdBipedTurn.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "CmdBipedTurn.h"

#include <source/axes/NUVAxes.h>
#include <source/level/objects/components/ICmpOrientation.h>

namespace hesp {

//#################### CONSTRUCTORS ####################
CmdBipedTurn::CmdBipedTurn(const ObjectID& objectID, int mouseMotionX, int mouseMotionY)
:	m_objectID(objectID), m_mouseMotionX(mouseMotionX), m_mouseMotionY(mouseMotionY)
{}

//#################### PUBLIC METHODS ####################
void CmdBipedTurn::execute(const ObjectManager_Ptr& objectManager, const std::vector<CollisionPolygon_Ptr>& polygons, const OnionTree_CPtr& tree,
						   const NavManager_CPtr& navManager, int milliseconds)
{
	ICmpOrientation_Ptr cmpOrientation = objectManager->get_component(m_objectID, cmpOrientation);
	NUVAxes_Ptr nuvAxes = cmpOrientation->nuv_axes();

	const double ROTATE_SPEED = 15.0;

	// For left-right motion, rotate about the (0,0,1) axis.
	nuvAxes->rotate(Vector3d(0,0,1), -m_mouseMotionX * ROTATE_SPEED * (milliseconds/1000.0));

	// For up-down motion, rotate about the u axis. Note that this rotation is clamped internally if necessary
	// to prevent the n vector being parallel to (0,0,1).
	nuvAxes->rotate(nuvAxes->u(), m_mouseMotionY * ROTATE_SPEED * (milliseconds/1000.0));
}

}
