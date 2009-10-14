/***
 * hesperus: PhysicsObject.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "PhysicsObject.h"

namespace hesp {

//#################### CONSTRUCTORS ####################
PhysicsObject::PhysicsObject(double inverseMass, PhysicsMaterial material, const Vector3d& position,
							 const Vector3d& velocity)
:	m_id(-1), m_inverseMass(inverseMass), m_material(material), m_position(position), m_velocity(velocity), m_sleeping(false)
{}

//#################### DESTRUCTOR ####################
PhysicsObject::~PhysicsObject() {}

//#################### PUBLIC METHODS ####################
const Vector3d& PhysicsObject::accumulated_force() const					{ return m_accumulatedForce; }
void PhysicsObject::apply_force(const Vector3d& force)						{ m_accumulatedForce += force; }
void PhysicsObject::cache_previous_position()								{ m_previousPosition = m_position; }
void PhysicsObject::clear_accumulated_force()								{ m_accumulatedForce = Vector3d(0,0,0); }
double PhysicsObject::inverse_mass() const									{ return m_inverseMass; }
bool PhysicsObject::is_sleeping() const										{ return m_sleeping; }
PhysicsMaterial PhysicsObject::material() const								{ return m_material; }
const Vector3d& PhysicsObject::position() const								{ return m_position; }
const boost::optional<Vector3d>& PhysicsObject::previous_position() const	{ return m_previousPosition; }
void PhysicsObject::set_position(const Vector3d& position)					{ m_position = position; }
void PhysicsObject::set_sleeping(bool sleeping)								{ m_sleeping = sleeping; }
void PhysicsObject::set_velocity(const Vector3d& velocity)					{ m_velocity = velocity; }
const Vector3d& PhysicsObject::velocity() const								{ return m_velocity; }

//#################### PRIVATE METHODS ####################
int PhysicsObject::id() const												{ return m_id; }
void PhysicsObject::set_id(int id)											{ m_id = id; }

}
