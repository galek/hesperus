/***
 * hesperus: SphereBounds.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "SphereBounds.h"

#include <source/ogl/WrappedGL.h>
#include <gl/glu.h>

#include <source/math/geom/GeomUtil.h>
#include <source/math/geom/Sphere.h>

namespace hesp {

//#################### CONSTRUCTORS ####################
SphereBounds::SphereBounds(double radius)
:	m_radius(radius)
{}

//#################### PUBLIC METHODS ####################
double SphereBounds::brush_expansion_distance(const Vector3d&) const
{
	return m_radius;
}

boost::optional<std::pair<Vector3d,Vector3d> > SphereBounds::determine_halfray_intersection(const Vector3d& s, const Vector3d& v) const
{
	Sphere sphere(Vector3d(0,0,0), m_radius);
	return determine_halfray_intersection_with_shape(s, v, sphere);
}

double SphereBounds::height() const
{
	return m_radius * 2;
}

void SphereBounds::render(const Vector3d& pos) const
{
	// FIXME:	If we want to use this for items, we should render the bounds as a circle, not a sphere,
	//			to make sure the item is still visible when highlighted.

	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	glColor3d(1,1,0);

	GLUquadricObj *quadric = gluNewQuadric();

	glPushMatrix();
		glTranslated(pos.x, pos.y, pos.z);
		gluSphere(quadric, m_radius, 16, 16);
	glPopMatrix();

	gluDeleteQuadric(quadric);

	glPopAttrib();
}

}
