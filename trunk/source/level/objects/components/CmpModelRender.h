/***
 * hesperus: CmpModelRender.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_CMPMODELRENDER
#define H_HESP_CMPMODELRENDER

#include "ICmpModelRender.h"

namespace hesp {

//#################### FORWARD DECLARATIONS ####################
typedef shared_ptr<class AnimationController> AnimationController_Ptr;
typedef shared_ptr<const class ConfiguredPose> ConfiguredPose_CPtr;
typedef shared_ptr<class Model> Model_Ptr;
typedef shared_ptr<const class Model> Model_CPtr;
typedef shared_ptr<class Skeleton> Skeleton_Ptr;
typedef shared_ptr<const class Skeleton> Skeleton_CPtr;

class CmpModelRender : public virtual ICmpModelRender
{
	//#################### PROTECTED VARIABLES ####################
protected:
	AnimationController_Ptr m_animController;
	bool m_highlights;
	ModelManager_Ptr m_modelManager;
	std::string m_modelName;
	ConfiguredPose_CPtr m_modelPose;

	//#################### CONSTRUCTORS ####################
protected:
	explicit CmpModelRender(const std::string& modelName);

	//#################### PUBLIC METHODS ####################
public:
	void set_highlights(bool enabled);
	void set_model_manager(const ModelManager_Ptr& modelManager);
	void set_skeleton();

	//#################### PROTECTED METHODS ####################
protected:
	const Model_Ptr& model();
	Model_CPtr model() const;
	void render_bounds(const Vector3d& p) const;
	static void render_nuv_axes(const Vector3d& p, const Vector3d& n, const Vector3d& u, const Vector3d& v);
	const Skeleton_Ptr& skeleton();
	Skeleton_CPtr skeleton() const;
};

}

#endif
