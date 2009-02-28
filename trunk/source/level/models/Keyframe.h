/***
 * hesperus: Keyframe.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_KEYFRAME
#define H_HESP_KEYFRAME

#include <vector>

#include <source/math/matrices/Matrix44.h>

namespace hesp {

class Keyframe
{
	//#################### PRIVATE VARIABLES ####################
private:
	double m_time;
	std::vector<Matrix44> m_boneMatrices;

	// TODO
};

}

#endif