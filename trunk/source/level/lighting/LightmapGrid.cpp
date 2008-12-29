/***
 * hesperus: LightmapGrid.cpp
 * Copyright Stuart Golodetz, 2008. All rights reserved.
 ***/

#include "LightmapGrid.h"

namespace hesp {

//#################### PUBLIC METHODS ####################
/**
Calculates the lightmap of light shining on the polygon from a particular point source.
The idea is that we'll ultimately combine the lightmaps from each of the different
lights in the scene to make the final lightmap for the polygon.

@param light	The point light source
@param tree		The BSP tree for the level
@return			The lightmap shining on the polygon from the given light source
*/
Lightmap_Ptr LightmapGrid::lightmap_from_light(const Light& light, const BSPTree_Ptr& tree) const
{
	// NYI
	throw 23;
}

//#################### PRIVATE METHODS ####################
/**
Find the best axis plane onto which to project the polygon.

@param n	The polygon's normal
@return		The best axis plane
*/
LightmapGrid::AxisPlane LightmapGrid::find_best_axis_plane(const Vector3d& n)
{
	double absX = fabs(n.x), absY = fabs(n.y), absZ = fabs(n.z);

	AxisPlane bestAxisPlane = YZ_PLANE;
	double absLargest = absX;
	if(absY > absLargest)
	{
		bestAxisPlane = XZ_PLANE;
		absLargest = absY;
	}
	if(absZ > absLargest) bestAxisPlane = XY_PLANE;

	return bestAxisPlane;
}

/**
Makes an appropriately-sized grid around the projected vertices, and calculates
lightmap coordinates for each vertex.

@param projectedVertices		The projection of the original polygon's vertices onto the best axis plane
@param axisPlane				The axis plane
@param vertexLightmapCoords		Used to return lightmap coordinates for each vertex to the caller
*/
void LightmapGrid::make_planar_grid(const std::vector<Vector2d>& projectedVertices, AxisPlane axisPlane, std::vector<TexCoords>& vertexLightmapCoords)
{
	// Determine a 2D AABB for the projected vertices.
	double minX, minY, maxX, maxY;
	minX = minY = INT_MAX;
	maxX = maxY = INT_MIN;

	const int vertCount = static_cast<int>(projectedVertices.size());
	for(int i=0; i<vertCount; ++i)
	{
		const double& x = projectedVertices[i].x;
		const double& y = projectedVertices[i].y;
		if(x < minX) minX = x;
		if(x > maxX) maxX = x;
		if(y < minY) minY = y;
		if(y > maxY) maxY = y;
	}

	double width = maxX - minX, height = maxY - minY;

	// Figure out a good size for the lightmap. Start with 8x8 lumels, and subdivide if each lumel would be too large.
	// (Note for the curious: lumel is short for 'luminance element'.)
	const double MAX_LUMEL_SIZE = 0.5;
	int lumelsX = 8;
	int lumelsY = 8;
	double lumelWidth = width / lumelsX, lumelHeight = height / lumelsY;
	while(lumelWidth > MAX_LUMEL_SIZE && lumelsX < 64)
	{
		lumelWidth /= 2;
		lumelsX *= 2;
	}
	while(lumelHeight > MAX_LUMEL_SIZE && lumelsY < 64)
	{
		lumelHeight /= 2;
		lumelsY *= 2;
	}

	// Construct the planar lightmap grid.
	m_grid.resize(lumelsY+1);
	for(int i=0; i<=lumelsY; ++i)
	{
		m_grid[i].reserve(lumelsX+1);
		double y = minY + i * lumelHeight;
		for(int j=0; j<=lumelsX; ++j)
		{
			double x = minX + j * lumelWidth;
			m_grid[i].push_back(planar_to_real(Vector2d(x,y), axisPlane));
		}
	}

	// Calculate the vertex lightmap coordinates.
	vertexLightmapCoords.reserve(vertCount);
	for(int i=0; i<vertCount; ++i)
	{
		double lu = (projectedVertices[i].x - minX) / width;
		double lv = (projectedVertices[i].y - minY) / height;
		vertexLightmapCoords.push_back(TexCoords(lu, lv));
	}
}

/**
Converts a vertex lying in one of the axis planes back into its 3D representation.

@param v			The planar vertex
@param axisPlane	The axis plane
@return				The 3D representation of the vertex
*/
Vector3d LightmapGrid::planar_to_real(const Vector2d& v, AxisPlane axisPlane)
{
	switch(axisPlane)
	{
		case YZ_PLANE:
			return Vector3d(0, v.x, v.y);
		case XZ_PLANE:
			return Vector3d(v.x, 0, v.y);
		default:	// case XY_PLANE
			return Vector3d(v.x, v.y, 0);
	}
}

/**
Project the vertex onto a given axis plane.

@param v		The vertex
@param axis		The axis plane
@return			The projected vertex
*/
Vector2d LightmapGrid::project_vertex_onto(const Vector3d& v, AxisPlane axisPlane)
{
	switch(axisPlane)
	{
		case YZ_PLANE:
		{
			// Project onto x = 0, i.e. the y-z plane.
			return Vector2d(v.y, v.z);
		}
		case XZ_PLANE:
		{
			// Project onto y = 0, i.e. the x-z plane.
			return Vector2d(v.x, v.z);
		}
		default:	// case XY_PLANE
		{
			// Project onto z = 0, i.e. the x-y plane.
			return Vector2d(v.x, v.y);
		}
	}
}

}
