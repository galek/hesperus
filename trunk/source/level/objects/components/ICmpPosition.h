/***
 * hesperus: ICmpPosition.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_ICMPPOSITION
#define H_HESP_ICMPPOSITION

#include <source/level/objects/base/IComponent.h>
#include <source/math/vectors/Vector3.h>

namespace hesp {

class ICmpPosition : public IComponent
{
	//#################### PUBLIC ABSTRACT METHODS ####################
public:
	virtual const Vector3d& position() const = 0;
	virtual void set_position(const Vector3d& position) = 0;

	//#################### PUBLIC METHODS ####################
public:
	std::string group_type() const			{ return "Position"; }
	static std::string static_group_type()	{ return "Position"; }

	std::string own_type() const			{ return "Position"; }
	static std::string static_own_type()	{ return "Position"; }
};

//#################### TYPEDEFS ####################
typedef shared_ptr<ICmpPosition> ICmpPosition_Ptr;
typedef shared_ptr<const ICmpPosition> ICmpPosition_CPtr;

}

#endif
