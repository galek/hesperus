/***
 * hesperus: AbsorbProjectileContactResolver.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "AbsorbProjectileContactResolver.h"

#include <source/level/objects/base/ObjectManager.h>
#include <source/level/objects/components/ICmpProjectile.h>
#include <source/level/physics/Contact.h>
#include <source/level/physics/PhysicsObject.h>

namespace hesp {

//#################### CONSTRUCTORS ####################
AbsorbProjectileContactResolver::AbsorbProjectileContactResolver(ObjectManager *objectManager, PhysicsMaterial projectileMaterial)
:	m_objectManager(objectManager), m_projectileMaterial(projectileMaterial)
{}

//#################### PRIVATE METHODS ####################
void AbsorbProjectileContactResolver::resolve_object_object(const Contact& contact, const OnionTree_CPtr& tree) const
{
	// Determine which of the two objects involved in the contact is which by examining the materials involved.
	ObjectID projectile, other;
	if(contact.objectA().material() == m_projectileMaterial)
	{
		projectile = contact.objectA().owner();
		other = contact.objectB()->owner();
	}
	else if(contact.objectB()->material() == m_projectileMaterial)
	{
		projectile = contact.objectB()->owner();
		other = contact.objectA().owner();
	}
	else throw Exception("Inappropriate contact resolver: Neither of the objects involved in the contact had the specified projectile type");

	// Destroy the projectile, as long as the non-projectile object is not the one that fired it.
	ICmpProjectile_CPtr cmpProjectile = m_objectManager->get_component(projectile, cmpProjectile);
	if(cmpProjectile && (!cmpProjectile->firer().valid() || cmpProjectile->firer() != other))
	{
		m_objectManager->queue_for_destruction(projectile);
	}
}

void AbsorbProjectileContactResolver::resolve_object_world(const Contact& contact) const
{
	m_objectManager->queue_for_destruction(contact.objectA().owner());
}

}
