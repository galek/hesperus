/***
 * hesperus: Entity.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_ENTITY
#define H_HESP_ENTITY

#include "ICollisionComponent.h"
#include "IHealthComponent.h"
#include "ILookComponent.h"
#include "IPositionComponent.h"
#include "IVisibilityComponent.h"
#include "IYokeComponent.h"

namespace hesp {

class Entity
{
	//#################### PRIVATE VARIABLES ####################
private:
	int m_id;
	std::string m_entityClass;
	ICollisionComponent_Ptr m_collisionComponent;
	IHealthComponent_Ptr m_healthComponent;
	ILookComponent_Ptr m_lookComponent;
	IPositionComponent_Ptr m_positionComponent;
	IVisibilityComponent_Ptr m_visibilityComponent;
	IYokeComponent_Ptr m_yokeComponent;

	//#################### CONSTRUCTORS ####################
public:
	Entity(const std::string& entityClass);

	//#################### PUBLIC METHODS ####################
public:
	ICollisionComponent_Ptr collision_component();
	IHealthComponent_Ptr health_component();
	int id() const;
	ILookComponent_Ptr look_component();
	IPositionComponent_Ptr position_component();
	IVisibilityComponent_Ptr visibility_component();
	IYokeComponent_Ptr yoke_component();
	void save(std::ostream& os) const;
	void set_collision_component(const ICollisionComponent_Ptr& collisionComponent);
	void set_health_component(const IHealthComponent_Ptr& healthComponent);
	void set_id(int id);
	void set_look_component(const ILookComponent_Ptr& lookComponent);
	void set_position_component(const IPositionComponent_Ptr& positionComponent);
	void set_visibility_component(const IVisibilityComponent_Ptr& visibilityComponent);
	void set_yoke_component(const IYokeComponent_Ptr& yokeComponent);
};

//#################### TYPEDEFS ####################
typedef shared_ptr<Entity> Entity_Ptr;

}

#endif
