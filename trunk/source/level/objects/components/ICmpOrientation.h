/***
 * hesperus: ICmpOrientation.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_ICMPORIENTATION
#define H_HESP_ICMPORIENTATION

#include <source/axes/NUVAxes.h>
#include <source/level/objects/base/IComponent.h>

namespace hesp {

class ICmpOrientation : public IComponent
{
	//#################### PUBLIC ABSTRACT METHODS ####################
public:
	virtual NUVAxes_Ptr nuv_axes() const = 0;

	//#################### PUBLIC METHODS ####################
public:
	std::string group_type() const			{ return "Orientation"; }
	static std::string static_group_type()	{ return "Orientation"; }

	std::string own_type() const			{ return "Orientation"; }
	static std::string static_own_type()	{ return "Orientation"; }
};

//#################### TYPEDEFS ####################
typedef shared_ptr<ICmpOrientation> ICmpOrientation_Ptr;
typedef shared_ptr<const ICmpOrientation> ICmpOrientation_CPtr;

}

#endif