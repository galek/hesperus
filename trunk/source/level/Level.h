/***
 * hesperus: Level.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_LEVEL
#define H_HESP_LEVEL

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <source/level/portals/OnionPortal.h>
#include <source/level/portals/Portal.h>
#include <source/level/vis/VisTable.h>
#include <source/util/PolygonTypes.h>

namespace hesp {

//#################### FORWARD DECLARATIONS ####################
typedef shared_ptr<class BSPTree> BSPTree_Ptr;
typedef shared_ptr<const class BSPTree> BSPTree_CPtr;
typedef shared_ptr<class GeometryRenderer> GeometryRenderer_Ptr;
typedef shared_ptr<const class GeometryRenderer> GeometryRenderer_CPtr;
typedef shared_ptr<class ModelManager> ModelManager_Ptr;
typedef shared_ptr<class NavDataset> NavDataset_Ptr;
typedef shared_ptr<class ObjectManager> ObjectManager_Ptr;
typedef shared_ptr<class OnionTree> OnionTree_Ptr;
typedef shared_ptr<const class OnionTree> OnionTree_CPtr;

class Level
{
	//#################### TYPEDEFS ####################
private:
	typedef std::vector<CollisionPolygon_Ptr> ColPolyVector;
	typedef std::vector<NavDataset_Ptr> NavDatasetVector;
	typedef std::vector<OnionPortal_Ptr> OnionPortalVector;
	typedef std::vector<Portal_Ptr> PortalVector;

	//#################### PRIVATE VARIABLES ####################
private:
	GeometryRenderer_Ptr m_geomRenderer;
	BSPTree_Ptr m_tree;
	PortalVector m_portals;
	LeafVisTable_Ptr m_leafVis;
	ColPolyVector m_onionPolygons;
	OnionTree_Ptr m_onionTree;
	OnionPortalVector m_onionPortals;
	NavDatasetVector m_navDatasets;
	ObjectManager_Ptr m_objectManager;
	ModelManager_Ptr m_modelManager;

	//#################### CONSTRUCTORS ####################
public:
	Level(const GeometryRenderer_Ptr& geomRenderer, const BSPTree_Ptr& tree,
		  const PortalVector& portals, const LeafVisTable_Ptr& leafVis,
		  const ColPolyVector& onionPolygons, const OnionTree_Ptr& onionTree,
		  const OnionPortalVector& onionPortals, const NavDatasetVector& navDatasets,
		  const ObjectManager_Ptr& objectManager, const ModelManager_Ptr& modelManager);

	//#################### PUBLIC METHODS ####################
public:
	BSPTree_CPtr bsp_tree() const;
	std::vector<int> find_visible_leaves(const Vector3d& eye) const;
	GeometryRenderer_CPtr geom_renderer() const;
	const std::vector<NavDataset_Ptr>& nav_datasets() const;
	const ObjectManager_Ptr& object_manager();
	const ColPolyVector& onion_polygons() const;
	OnionTree_CPtr onion_tree() const;
	const PortalVector& portals() const;
};

//#################### TYPEDEFS ####################
typedef shared_ptr<Level> Level_Ptr;
typedef shared_ptr<const Level> Level_CPtr;

}

#endif
