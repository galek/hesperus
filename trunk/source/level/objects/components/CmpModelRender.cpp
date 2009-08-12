/***
 * hesperus: CmpModelRender.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "CmpModelRender.h"

#include "ICmpAABBBounds.h"
#include "ICmpAnimChooser.h"
#include "ICmpOrientation.h"
#include "ICmpPosition.h"

namespace hesp {

//#################### CONSTRUCTORS ####################
CmpModelRender::CmpModelRender(const std::string& modelName)
:	m_modelName(modelName), m_animController(new AnimationController), m_highlights(false)
{}

//#################### STATIC FACTORY METHODS ####################
IObjectComponent_Ptr CmpModelRender::load(const Properties& properties)
{
	return IObjectComponent_Ptr(new CmpModelRender(properties.get<std::string>("ModelName")));
}

//#################### PUBLIC METHODS ####################
AnimationController_CPtr CmpModelRender::anim_controller() const
{
	return m_animController;
}

void CmpModelRender::check_dependencies() const
{
	check_dependency<ICmpOrientation>();
	check_dependency<ICmpPosition>();
}

const std::string& CmpModelRender::model_name() const
{
	return m_modelName;
}

void CmpModelRender::render() const
{
	ICmpOrientation_Ptr cmpOrientation = m_objectManager->get_component(m_objectID, cmpOrientation);
	ICmpPosition_Ptr cmpPosition = m_objectManager->get_component(m_objectID, cmpPosition);

	const Vector3d& p = cmpPosition->position();
	const Vector3d& n = cmpOrientation->nuv_axes()->n();
	const Vector3d& u = cmpOrientation->nuv_axes()->u();
	const Vector3d& v = cmpOrientation->nuv_axes()->v();

	RBTMatrix_CPtr mat = construct_model_matrix(p, n, u, v);

	Model_Ptr model = m_modelManager->model(m_modelName);
	model->apply_pose_to_skeleton(m_animController);

	// Render the model.
	glPushMatrix();
		glMultMatrixd(&mat->rep()[0]);
		model->render();
	glPopMatrix();

	if(m_highlights)
	{
		// If the object should be highlighted, render the object's AABB.
		render_aabb(p);
	}

	// Render the object's NUV axes.
	render_nuv_axes(p, n, u, v);
}

std::pair<std::string,Properties> CmpModelRender::save() const
{
	Properties properties;
	properties.set("ModelName", m_modelName);
	return std::make_pair("ModelRender", properties);
}

void CmpModelRender::set_highlights(bool enabled)
{
	m_highlights = enabled;
}

void CmpModelRender::set_model_manager(const ModelManager_Ptr& modelManager)
{
	m_modelManager = modelManager;
	m_modelManager->register_model(m_modelName);
}

void CmpModelRender::set_skeleton()
{
	Skeleton_Ptr skeleton = m_modelManager->model(m_modelName)->skeleton();
	m_animController->set_skeleton(skeleton);
}

void CmpModelRender::update_animation(int milliseconds, const std::vector<CollisionPolygon_Ptr>& polygons, const OnionTree_Ptr& tree, const std::vector<NavDataset_Ptr>& navDatasets)
{
	ICmpAnimChooser_Ptr cmpAnimChooser = m_objectManager->get_component(m_objectID, cmpAnimChooser);
	if(cmpAnimChooser) m_animController->request_animation(cmpAnimChooser->choose_animation(polygons, tree, navDatasets));

	m_animController->update(milliseconds);
}

//#################### PRIVATE METHODS ####################
RBTMatrix_CPtr CmpModelRender::construct_model_matrix(const Vector3d& p, const Vector3d& n, const Vector3d& u, const Vector3d& v)
{
	// Note:	This matrix maps x -> u, -y -> n, z -> v, and translates by p. Since models are
	//			built in Blender facing in the -y direction, this turns out to be exactly the
	//			transformation required to render the models with the correct position and
	//			orientation.
	RBTMatrix_Ptr mat = RBTMatrix::zeros();
	RBTMatrix& m = *mat;
	m(0,0) = u.x;		m(0,1) = -n.x;		m(0,2) = v.x;		m(0,3) = p.x;
	m(1,0) = u.y;		m(1,1) = -n.y;		m(1,2) = v.y;		m(1,3) = p.y;
	m(2,0) = u.z;		m(2,1) = -n.z;		m(2,2) = v.z;		m(2,3) = p.z;

	return mat;
}

void CmpModelRender::render_aabb(const Vector3d& p) const
{
	ICmpAABBBounds_Ptr cmpBounds = m_objectManager->get_component(m_objectID, cmpBounds);
	if(!cmpBounds) return;

	const AABB3d& aabb = m_objectManager->aabbs()[cmpBounds->cur_aabb_index()];
	AABB3d tAABB = aabb.translate(p);
	const Vector3d& mins = tAABB.minimum();
	const Vector3d& maxs = tAABB.maximum();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	glColor3d(1,1,0);

	glBegin(GL_QUADS);
		// Front
		glVertex3d(mins.x, mins.y, mins.z);
		glVertex3d(maxs.x, mins.y, mins.z);
		glVertex3d(maxs.x, mins.y, maxs.z);
		glVertex3d(mins.x, mins.y, maxs.z);

		// Right
		glVertex3d(maxs.x, mins.y, mins.z);
		glVertex3d(maxs.x, maxs.y, mins.z);
		glVertex3d(maxs.x, maxs.y, maxs.z);
		glVertex3d(maxs.x, mins.y, maxs.z);

		// Back
		glVertex3d(maxs.x, maxs.y, mins.z);
		glVertex3d(mins.x, maxs.y, mins.z);
		glVertex3d(mins.x, maxs.y, maxs.z);
		glVertex3d(maxs.x, maxs.y, maxs.z);

		// Left
		glVertex3d(mins.x, maxs.y, mins.z);
		glVertex3d(mins.x, mins.y, mins.z);
		glVertex3d(mins.x, mins.y, maxs.z);
		glVertex3d(mins.x, maxs.y, maxs.z);
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
}

void CmpModelRender::render_nuv_axes(const Vector3d& p, const Vector3d& n, const Vector3d& u, const Vector3d& v) const
{
	Vector3d pn = p + n;
	Vector3d pu = p + u;
	Vector3d pv = p + v;
	glBegin(GL_LINES);
		glColor3d(1,0,0);	glVertex3d(p.x, p.y, p.z);	glVertex3d(pn.x, pn.y, pn.z);
		glColor3d(0,1,0);	glVertex3d(p.x, p.y, p.z);	glVertex3d(pu.x, pu.y, pu.z);
		glColor3d(0,0,1);	glVertex3d(p.x, p.y, p.z);	glVertex3d(pv.x, pv.y, pv.z);
	glEnd();
}

}
