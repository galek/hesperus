/***
 * hesperus: CmpOwnable.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "CmpOwnable.h"

namespace hesp {

//#################### CONSTRUCTORS ####################
CmpOwnable::CmpOwnable(const std::string& attachPoint, const ObjectID& owner)
:	m_attachPoint(attachPoint), m_owner(owner)
{}

//#################### STATIC FACTORY METHODS ####################
IObjectComponent_Ptr CmpOwnable::load(const Properties& properties)
{
	return IObjectComponent_Ptr(new CmpOwnable(properties.get<std::string>("AttachPoint"), properties.get<ObjectID>("Owner")));
}

//#################### PUBLIC METHODS ####################
const std::string& CmpOwnable::attach_point() const
{
	return m_attachPoint;
}

void CmpOwnable::clear_owner()
{
	m_owner = ObjectID();
}

const ObjectID& CmpOwnable::owner() const
{
	return m_owner;
}

std::pair<std::string,Properties> CmpOwnable::save() const
{
	Properties properties;
	properties.set("AttachPoint", m_attachPoint);
	properties.set("Owner", m_owner);
	return std::make_pair("Ownable", properties);
}

void CmpOwnable::set_owner(const ObjectID& owner)
{
	m_owner = owner;
}

}
