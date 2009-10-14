/***
 * hesperus: Contact.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_CONTACT
#define H_HESP_CONTACT

#include <boost/optional.hpp>

#include <source/math/vectors/Vector3.h>

namespace hesp {

//#################### FORWARD DECLARATIONS ####################
class PhysicsObject;

class Contact
{
	//#################### PRIVATE VARIABLES ####################
private:
	Vector3d m_normal;							// the normal at the contact point
	PhysicsObject& m_objectA;					// the first object involved in the collision
	boost::optional<PhysicsObject&> m_objectB;	// the second object involved in the collision (if any)
	double m_penetrationDepth;					// the depth to which the objects have interpenetrated
	Vector3d m_point;							// the position of the contact (in world coordinates)
	double m_time;								// a number in [0,1] indicating when in the frame the contact occurred

	//#################### CONSTRUCTORS ####################
public:
	Contact(const Vector3d& point, const Vector3d& normal, double penetrationDepth, double time, PhysicsObject& objectA, const boost::optional<PhysicsObject&>& objectB = boost::none);

	//#################### PUBLIC METHODS ####################
public:
	const Vector3d& normal() const;
	PhysicsObject& objectA() const;
	const boost::optional<PhysicsObject&>& objectB() const;
	double penetration_depth() const;
	const Vector3d& point() const;
	double time() const;
};

}

#endif
