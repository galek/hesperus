/***
 * hesperus: WalkLink.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "WalkLink.h"

#include <sstream>

#include <source/ogl/WrappedGL.h>

#include <source/math/geom/GeomUtil.h>

namespace hesp {

//#################### CONSTRUCTORS ####################
WalkLink::WalkLink(int sourcePoly, int destPoly, const Vector3d& p1, const Vector3d& p2)
:	NavLink(sourcePoly, destPoly), m_edge(p1, p2)
{}

WalkLink::WalkLink(int sourcePoly, int destPoly, const LineSegment3d& edge)
:	NavLink(sourcePoly, destPoly), m_edge(edge)
{}

//#################### PUBLIC METHODS ####################
Vector3d WalkLink::dest_position() const
{
	return (m_edge.e1 + m_edge.e2) / 2;
}

Vector3d_Ptr WalkLink::hit_test(const Vector3d& s, const Vector3d& d) const
{
	Plane plane = *make_axial_plane(m_edge.e1, m_edge.e2, Vector3d(0,0,1));

	// Make sure we don't try and determine the intersection if the entity's walking parallel to the plane.
	Vector3d v = d - s;
	if(fabs(v.dot(plane.normal())) < EPSILON) return Vector3d_Ptr();

	std::pair<Vector3d,bool> hit = determine_linesegment_intersection_with_plane(s, d, plane);

	// FIXME: We need to check that the point's within the link edge.
	if(hit.second) return Vector3d_Ptr(new Vector3d(hit.first));
	else return Vector3d_Ptr();
}

NavLink_Ptr WalkLink::load(const std::string& data)
{
	std::stringstream ss;
	ss << data;

	std::ios_base::fmtflags oldFlags = ss.flags();
	ss.setf(std::ios_base::skipws);

	int sourcePoly, destPoly;
	LineSegment3d edge;

	ss >> sourcePoly >> destPoly >> edge;

	ss.flags(oldFlags);

	return NavLink_Ptr(new WalkLink(sourcePoly, destPoly, edge));
}

void WalkLink::output(std::ostream& os) const
{
	os << "Walk " << m_sourcePoly << ' ' << m_destPoly << ' ' << m_edge;
}

void WalkLink::render() const
{
	Vector3d mid = (m_edge.e1 + m_edge.e2) / 2;
	Vector3d alongEdge = (m_edge.e2 - m_edge.e1);
	Vector3d acrossEdge = alongEdge.cross(Vector3d(0,0,1)).normalize();
	acrossEdge *= 0.2;
	Vector3d s = mid + acrossEdge, d = mid - acrossEdge;
	glBegin(GL_LINES);
		glColor3d(0,1,1);
		glVertex3d(s.x, s.y, s.z);
		glVertex3d(d.x, d.y, d.z);
	glEnd();
}

Vector3d WalkLink::source_position() const
{
	return (m_edge.e1 + m_edge.e2) / 2;
}

double WalkLink::traversal_time(double traversalSpeed) const
{
	return 0;
}

Vector3d WalkLink::traverse(const Vector3d& source, double t) const
{
	return source;
}

}
