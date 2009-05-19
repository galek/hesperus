/***
 * hesperus: CmpRender.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "CmpRender.h"

namespace hesp {

//#################### CONSTRUCTORS ####################
CmpRender::CmpRender(const std::string& modelName)
:	m_modelName(modelName), m_animController(new AnimationController)
{}

//#################### STATIC FACTORY METHODS ####################
IComponent_Ptr CmpRender::create(const Properties& properties)
{
	return IComponent_Ptr(new CmpRender(properties.get_actual<std::string>("ModelName")));
}

//#################### PUBLIC METHODS ####################
AnimationController_Ptr CmpRender::anim_controller() const
{
	return m_animController;
}

const std::string& CmpRender::model_name() const
{
	return m_modelName;
}

}
