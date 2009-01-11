/***
 * hesperus: OnionPortalsFileUtil.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_IO_ONIONPORTALSFILEUTIL
#define H_HESP_IO_ONIONPORTALSFILEUTIL

#include <string>
#include <vector>

#include <source/level/portals/OnionPortal.h>

namespace hesp {

struct OnionPortalsFileUtil
{
	//#################### LOADING METHODS ####################
	// TODO

	//#################### SAVING METHODS ####################
	static void save(const std::string& filename, const std::vector<OnionPortal_Ptr>& portals);
};

}

#endif
