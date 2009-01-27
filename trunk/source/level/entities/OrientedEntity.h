/***
 * hesperus: OrientedEntity.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_ORIENTEDENTITY
#define H_HESP_ORIENTEDENTITY

#include <source/math/vectors/Vector3.h>
#include "Entity.h"

namespace hesp {

/**
This class represents an oriented entity (i.e. the entity has a look vector).
*/
class OrientedEntity : virtual public Entity
{
	//#################### PROTECTED VARIABLES ####################
protected:
	Vector3d m_look;

	//#################### CONSTRUCTORS ####################
public:
	OrientedEntity(const Vector3d& look)
	:	m_look(look)
	{}

	//#################### DESTRUCTOR ####################
public:
	virtual ~OrientedEntity() {}

	//#################### PUBLIC METHODS ####################
public:
	OrientedEntity *as_oriented()
	{
		return this;
	}

	const Vector3d& look() const
	{
		return m_look;
	}
};

}

#endif
