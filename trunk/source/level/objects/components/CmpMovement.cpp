/***
 * hesperus: CmpMovement.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "CmpMovement.h"

#include <source/level/bounds/BoundsManager.h>
#include <source/level/nav/NavDataset.h>
#include <source/level/nav/NavLink.h>
#include <source/level/nav/NavMesh.h>
#include <source/level/nav/NavMeshUtil.h>
#include <source/level/nav/NavPolygon.h>
#include <source/math/geom/GeomUtil.h>
#include "ICmpSimulation.h"

namespace hesp {

//#################### CONSTRUCTORS ####################
CmpMovement::CmpMovement()
:	m_curNavPolyIndex(-1)
{}

//#################### STATIC FACTORY METHODS ####################
IObjectComponent_Ptr CmpMovement::load(const Properties&)
{
	return IObjectComponent_Ptr(new CmpMovement);
}

//#################### PUBLIC METHODS ####################
bool CmpMovement::attempt_navmesh_acquisition(const std::vector<CollisionPolygon_Ptr>& polygons, const OnionTree_CPtr& tree, const NavMesh_CPtr& navMesh)
{
	ICmpPosition_CPtr cmpPosition = m_objectManager->get_component(m_objectID, cmpPosition);	assert(cmpPosition != NULL);
	const Vector3d& position = cmpPosition->position();

	// Try and find a nav polygon, starting from the last known one.
	m_curNavPolyIndex = NavMeshUtil::find_nav_polygon(position, m_curNavPolyIndex, polygons, tree, navMesh);

	return m_curNavPolyIndex != -1;
}

void CmpMovement::move(const Vector3d& dir, double speed, int milliseconds, const std::vector<CollisionPolygon_Ptr>& polygons, const OnionTree_CPtr& tree,
					   const std::vector<NavDataset_Ptr>& navDatasets)
{
	ICmpSimulation_Ptr cmpSimulation = m_objectManager->get_component(m_objectID, cmpSimulation);	assert(cmpSimulation != NULL);

	Move move;
	move.dir = dir;
	move.mapIndex = m_objectManager->bounds_manager()->lookup_bounds_index(cmpSimulation->bounds_group(), cmpSimulation->posture());
	move.timeRemaining = milliseconds / 1000.0;

	NavMesh_Ptr navMesh = navDatasets[move.mapIndex]->nav_mesh();

	double oldTimeRemaining;
	do
	{
		oldTimeRemaining = move.timeRemaining;

		if(m_curTraversal) do_traverse_move(move, speed /* FIXME: Select the appropriate speed here */, polygons, navMesh);
		if(move.timeRemaining == 0) break;

		if(attempt_navmesh_acquisition(polygons, tree, navMesh)) do_navmesh_move(move, speed, polygons, tree, navMesh);
		else do_direct_move(move, speed, tree);
	} while(move.timeRemaining > 0 && oldTimeRemaining - move.timeRemaining > 0.0001);
}

Properties CmpMovement::save() const
{
	return Properties();
}

//#################### PRIVATE METHODS ####################
bool CmpMovement::do_direct_move(Move& move, double speed, const OnionTree_CPtr& tree)
{
	// NYI
	throw 23;
}

void CmpMovement::do_navmesh_move(Move& move, double speed, const std::vector<CollisionPolygon_Ptr>& polygons, const OnionTree_CPtr& tree, const NavMesh_CPtr& navMesh)
{
	ICmpPosition_Ptr cmpPosition = m_objectManager->get_component(m_objectID, cmpPosition);		assert(cmpPosition != NULL);

	// Step 1:		Project the movement vector onto the plane of the current nav polygon.

	const NavPolygon& navPoly = *navMesh->polygons()[m_curNavPolyIndex];
	int curColPolyIndex = navPoly.collision_poly_index();
	const CollisionPolygon& curPoly = *polygons[curColPolyIndex];
	Plane plane = make_plane(curPoly);
	move.dir = project_vector_onto_plane(move.dir, plane);
	if(move.dir.length_squared() > SMALL_EPSILON*SMALL_EPSILON) move.dir.normalize();

	// Step 2:		Check whether the new movement vector goes through the influence zone of any of the out navlinks.

	Vector3d source = cmpPosition->position();
	Vector3d dest = source + move.dir * speed * move.timeRemaining;

	boost::optional<Vector3d> hit;
	int hitNavlink = -1;
	const std::vector<int>& links = navPoly.out_links();
	for(std::vector<int>::const_iterator it=links.begin(), iend=links.end(); it!=iend; ++it)
	{
		const NavLink_Ptr& link = navMesh->links()[*it];
		hit = link->hit_test(source, dest);

		if(hit)
		{
			hitNavlink = *it;
#if 0
			std::cout << "Hit navlink at " << *hit << ": ";
			link->output(std::cout);
			std::cout << '\n';
#endif
			break;
		}
	}

	// Step 3.a:	If the new movement vector doesn't hit a navlink, check whether the other end of the movement vector is within the current polygon.
	//
	//				-	If yes, move there, set the time remaining to zero and return.
	//
	//				-	If no, do a direct move in the original direction, since we are either leaving the navmesh or hitting a wall.

	if(hitNavlink == -1)
	{
		if(point_in_polygon(dest, curPoly))
		{
			cmpPosition->set_position(dest);
			move.timeRemaining = 0;
		}
		else
		{
			do_direct_move(move, speed, tree);
		}

		return;
	}

	// Step 3.b:	If the new movement vector hits a navlink, move the point at which it first enters the influence zone,
	//				and reduce the time remaining appropriately. Then, initiate the link traversal.

	// Move the object to the link entrance point.
	cmpPosition->set_position(*hit);

	// Update the time remaining.
	double moveLength = source.distance(*hit);
	double timeTaken = moveLength / speed;
	move.timeRemaining -= timeTaken;

	// Initiate the link traversal.
	m_curTraversal.reset(new Traversal(hitNavlink, *hit, 0));
}

void CmpMovement::do_traverse_move(Move& move, double speed, const std::vector<CollisionPolygon_Ptr>& polygons, const NavMesh_CPtr& navMesh)
{
	ICmpPosition_Ptr cmpPosition = m_objectManager->get_component(m_objectID, cmpPosition);		assert(cmpPosition != NULL);

	Traversal_CPtr traversal = m_curTraversal;
	if(!traversal) return;

	NavLink_Ptr link = navMesh->links()[traversal->linkIndex];
	double remaining = 1 - traversal->t;													// % of link remaining
	double remainingTraversalTime = remaining * link->traversal_time(speed);				// time to traverse remainder
	double availableTraversalTime = std::min(remainingTraversalTime, move.timeRemaining);	// time to spend traversing

	if(availableTraversalTime >= remainingTraversalTime)
	{
		// Finish traversing the link:

		// Update the current nav polygon and clear the current traversal.
		m_curNavPolyIndex = link->dest_poly();
		m_curTraversal.reset();

		// Move to an exit point on the link.
		Vector3d dest = link->traverse(traversal->source, 1);
		cmpPosition->set_position(dest);
		move.timeRemaining -= remainingTraversalTime;
		
#if 0
		int colPolyIndex = navMesh->polygons()[link->dest_poly()]->collision_poly_index();
		std::cout << "Linked to polygon (" << colPolyIndex << ',' << link->dest_poly() << ')' << std::endl;
#endif

		// Move the object very slightly away from the navlink exit: this is a hack to prevent link loops.
		int destColPolyIndex = navMesh->polygons()[link->dest_poly()]->collision_poly_index();
		const CollisionPolygon& destPoly = *polygons[destColPolyIndex];
		Plane destPlane = make_plane(destPoly);
		Vector3d destDir = project_vector_onto_plane(move.dir, destPlane);
		dest += destDir * 0.001;
		if(point_in_polygon(dest, destPoly)) cmpPosition->set_position(dest);
	}
	else
	{
		// Work out how much further we've progressed and update the traversal field accordingly.
		double deltaT = (availableTraversalTime / remainingTraversalTime) * remaining;
		Traversal_CPtr newTraversal(new Traversal(traversal->linkIndex, traversal->source, traversal->t + deltaT));
		m_curTraversal = newTraversal;

		// Move further along the link.
		Vector3d dest = link->traverse(newTraversal->source, newTraversal->t);
		cmpPosition->set_position(dest);
		move.timeRemaining = 0;
	}
}

void CmpMovement::update_move_direction_for_sliding(Move& move)
{
	// NYI
	throw 23;
}

}
