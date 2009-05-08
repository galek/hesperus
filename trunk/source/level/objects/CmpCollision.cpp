/***
 * hesperus: CmpCollision.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "CmpCollision.h"

namespace hesp {

//#################### PUBLIC METHODS ####################
const std::vector<int>& CmpCollision::aabb_indices() const
{
	return m_aabbIndices;
}

int CmpCollision::pose() const
{
	return m_pose;
}

void CmpCollision::set_pose(int pose)
{
	m_pose = pose;
}

}
