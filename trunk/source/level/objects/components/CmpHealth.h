/***
 * hesperus: CmpHealth.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_CMPHEALTH
#define H_HESP_CMPHEALTH

#include "ICmpHealth.h"

namespace hesp {

class CmpHealth : public ICmpHealth
{
	//#################### PRIVATE VARIABLES ####################
private:
	int m_health;
	int m_maxHealth;

	//#################### CONSTRUCTORS ####################
public:
	explicit CmpHealth(int health, int maxHealth);

	//#################### STATIC FACTORY METHODS ####################
public:
	static IObjectComponent_Ptr load(const Properties& properties);

	//#################### PUBLIC METHODS ####################
public:
	int health() const;
	int max_health() const;
	std::pair<std::string,Properties> save() const;
	HealthStatus status() const;
};

}

#endif
