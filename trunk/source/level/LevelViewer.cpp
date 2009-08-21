/***
 * hesperus: LevelViewer.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "LevelViewer.h"

#include <gl/glu.h>

#include <source/colours/Colour3d.h>
#include <source/gui/Screen.h>
#include <source/level/objects/components/ICmpCharacterModelRender.h>

namespace hesp {

//#################### CONSTRUCTORS ####################
LevelViewer::LevelViewer(const Level_Ptr& level, const Camera_Ptr& camera)
:	m_level(level), m_camera(camera)
{}

//#################### PUBLIC METHODS ####################
void LevelViewer::render() const
{
	// Draw the level itself.
	Screen::instance().set_persp_viewport(*m_extents, 45.0, 0.1, 4096.0);
	render_level();

	// Draw a white border round the component on the screen.
	Screen::instance().set_ortho_viewport(*m_extents);
	glColor3d(1,1,1);
	render_extents();
}

//#################### PRIVATE METHODS ####################
void LevelViewer::render_level() const
{
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);

	// Enable back-face culling.
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// Enable the z-buffer.
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	// Set the view.
	m_camera->update();
	Vector3d eye = m_camera->eye(), at = m_camera->at(), up = m_camera->up();
	gluLookAt(eye.x, eye.y, eye.z, at.x, at.y, at.z, up.x, up.y, up.z);

	// Determine which leaves are potentially visible from the current viewer position.
	std::vector<int> visibleLeaves = m_level->find_visible_leaves(eye);

	// Make a list of all the polygons which need rendering and pass them to the renderer.
	BSPTree_CPtr tree = m_level->bsp_tree();
	std::vector<int> polyIndices;
	for(std::vector<int>::const_iterator it=visibleLeaves.begin(), iend=visibleLeaves.end(); it!=iend; ++it)
	{
		const BSPLeaf *leaf = tree->leaf(*it);
		std::copy(leaf->polygon_indices().begin(), leaf->polygon_indices().end(), std::back_inserter(polyIndices));
	}
	m_level->geom_renderer()->render(polyIndices);
#if 0
	std::cout << "Polygon Count " << polyIndices.size() << std::endl;
#endif

	// Render the navigation meshes.
#if 0
	render_navmeshes();
	render_navlinks();
#endif

	// Render the portals.
#if 0
	render_portals();
#endif

	// Render the visible objects. (These must be done after everything else to ensure that
	// things like the crosshair and active item are not obscured by the rest of the scene
	// when rendering in first-person.)
	render_objects();

	glPopAttrib();
}

void LevelViewer::render_navlinks() const
{
	const std::vector<NavDataset_Ptr>& navDatasets = m_level->nav_datasets();
	int datasetCount = static_cast<int>(navDatasets.size());
	for(int i=0; i<datasetCount; ++i)
	{
		const std::vector<NavLink_Ptr>& navLinks = navDatasets[i]->nav_mesh()->links();
		int linkCount = static_cast<int>(navLinks.size());
		for(int j=0; j<linkCount; ++j)
		{
			navLinks[j]->render();
		}
	}
}

void LevelViewer::render_navmeshes() const
{
	// Note: Navmeshes don't need to be rendered efficiently, since we only render them for testing purposes anyway.

	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);

	Colour3d colours[] = { Colour3d(1,0,0), Colour3d(0,1,0), Colour3d(0,0,1) };
	int colourCount = sizeof(colours)/sizeof(Colour3d);

	const std::vector<NavDataset_Ptr>& navDatasets = m_level->nav_datasets();
	const std::vector<CollisionPolygon_Ptr>& onionPolygons = m_level->onion_polygons();

	int datasetCount = static_cast<int>(navDatasets.size());
	for(int i=0; i<datasetCount; ++i)
	{
		int c = i % colourCount;
		glColor3d(colours[c].r, colours[c].g, colours[c].b);

		const std::vector<NavPolygon_Ptr>& navPolys = navDatasets[i]->nav_mesh()->polygons();
		int polyCount = static_cast<int>(navPolys.size());
		for(int j=0; j<polyCount; ++j)
		{
			const CollisionPolygon_Ptr& onionPoly = onionPolygons[navPolys[j]->collision_poly_index()];
			int vertCount = onionPoly->vertex_count();
			glBegin(GL_POLYGON);
				for(int k=0; k<vertCount; ++k)
				{
					const Vector3d& v = onionPoly->vertex(k);
					glVertex3d(v.x, v.y, v.z);
				}
			glEnd();
		}
	}

	glPopAttrib();
}

void LevelViewer::render_objects() const
{
	// FIXME: For performance reasons, we should only be rendering objects which are potentially visible.

	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);

	ObjectManager_Ptr objectManager = m_level->object_manager();
	std::vector<ObjectID> renderables = objectManager->group("Renderables");
	ICmpCharacterModelRender_Ptr cmpFirstPersonRender;
	for(size_t i=0, size=renderables.size(); i<size; ++i)
	{
		const ObjectID& renderable = renderables[i];
		if(renderable != objectManager->player() || !m_camera->is_inside_player())
		{
			ICmpRender_Ptr cmpRender = objectManager->get_component(renderable, cmpRender);
			cmpRender->render();
		}
		else
		{
			cmpFirstPersonRender = objectManager->get_component(renderable, cmpFirstPersonRender);
			if(!cmpFirstPersonRender) throw Exception("For the player to be renderable in first-person, it must have a CharacterModelRender component");
		}
	}

	// Note:	First-person rendering means rendering the active item (e.g. the weapon held by the player) but
	//			not the actual player model.
	if(cmpFirstPersonRender) cmpFirstPersonRender->render_first_person();

	glPopAttrib();
}

void LevelViewer::render_portals() const
{
	// Note: Portals don't need to be rendered efficiently, since we only render them for testing purposes anyway.

	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);

	glColor3d(1,1,1);

	const PortalVector& portals = m_level->portals();
	for(PortalVector::const_iterator it=portals.begin(), iend=portals.end(); it!=iend; ++it)
	{
		const Portal& portal = **it;
		int vertCount = portal.vertex_count();
		glBegin(GL_POLYGON);
			for(int j=0; j<vertCount; ++j)
			{
				const Vector3d& v = portal.vertex(j);
				glVertex3d(v.x, v.y, v.z);
			}
		glEnd();
	}

	glPopAttrib();
}

}
