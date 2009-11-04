/***
 * hesperus: CmpProjectileWeaponUsable.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_CMPPROJECTILEWEAPONUSABLE
#define H_HESP_CMPPROJECTILEWEAPONUSABLE

#include "CmpUsable.h"

namespace hesp {

class CmpProjectileWeaponUsable : public CmpUsable
{
	//#################### PRIVATE VARIABLES ####################
private:
	double m_muzzleSpeed;
	std::string m_projectileType;

	//#################### CONSTRUCTORS ####################
public:
	CmpProjectileWeaponUsable(const std::string& usableGroup, const std::vector<std::string>& hotspots, double muzzleSpeed, const std::string& projectileType);

	//#################### STATIC FACTORY METHODS ####################
public:
	static IObjectComponent_Ptr load(const Properties& properties);

	//#################### PUBLIC METHODS ####################
public:
	Properties save() const;
	void use();

	std::string own_type() const			{ return "ProjectileWeaponUsable"; }
	static std::string static_own_type()	{ return "ProjectileWeaponUsable"; }
};

}

#endif
