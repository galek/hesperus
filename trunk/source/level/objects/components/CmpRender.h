/***
 * hesperus: CmpRender.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_CMPRENDER
#define H_HESP_CMPRENDER

#include "ICmpRender.h"

namespace hesp {

class CmpRender : public ICmpRender
{
	//#################### PRIVATE VARIABLES ####################
private:
	AnimationController_Ptr m_animController;
	std::string m_modelName;

	//#################### CONSTRUCTORS ####################
public:
	explicit CmpRender(const std::string& modelName);

	//#################### STATIC FACTORY METHODS ####################
public:
	static IComponent_Ptr load(const Properties& properties);

	//#################### PUBLIC METHODS ####################
public:
	AnimationController_Ptr anim_controller() const;
	void check_dependencies() const;
	const std::string& model_name() const;
	std::pair<std::string,Properties> save() const;
};

}

#endif