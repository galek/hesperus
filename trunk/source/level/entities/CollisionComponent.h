/***
 * hesperus: CollisionComponent.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_COLLISIONCOMPONENT
#define H_HESP_COLLISIONCOMPONENT

#include <source/io/FieldIO.h>
#include "ICollisionComponent.h"

namespace hesp {

class CollisionComponent : public ICollisionComponent
{
	//#################### PRIVATE VARIABLES ####################
private:
	std::vector<int> m_aabbIndices;
	OnionTree::Transition_Ptr m_lastTransition;		// records the details of last time the entity would have crossed a wall into solid space (had we not stopped it)
	int m_pose;

	//#################### CONSTRUCTORS ####################
public:
	CollisionComponent(std::istream& is)
	{
		m_aabbIndices = FieldIO::read_intarray_field(is, "AABBs");
		m_pose = FieldIO::read_typed_field<int>(is, "Pose");
	}

	//#################### PUBLIC METHODS ####################
public:
	const std::vector<int>& aabb_indices() const
	{
		return m_aabbIndices;
	}

	const OnionTree::Transition_Ptr& last_transition() const
	{
		return m_lastTransition;
	}

	int pose() const
	{
		return m_pose;
	}

	void save(std::ostream& os) const
	{
		FieldIO::write_intarray_field(os, "AABBs", m_aabbIndices);
		FieldIO::write_typed_field(os, "Pose", m_pose);
	}

	void set_pose(int pose)
	{
		if(std::find(m_aabbIndices.begin(), m_aabbIndices.end(), pose) != m_aabbIndices.end())
		{
			m_pose = pose;
		}
	}

	void update_last_transition(const OnionTree::Transition_Ptr& lastTransition)
	{
		m_lastTransition = lastTransition;
	}
};

}

#endif
