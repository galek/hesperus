/***
 * hesperus: OnionCompiler.tpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include <source/math/geom/GeomUtil.h>
#include "OnionBranch.h"

namespace hesp {

//#################### CONSTRUCTORS ####################
template <typename Poly>
OnionCompiler<Poly>::OnionCompiler(const std::vector<PolyVector>& maps, double weight)
:	m_weight(weight), m_mapCount(static_cast<int>(maps.size())), m_polygons(new PolyVector)
{
	typedef std::map<OnionPlane_Ptr,int,OnionPlanePred> OnionPlaneMap;

	// Build the set of unique onion planes, and a map from polygons to their respective planes.
	OnionPlaneMap onionPlanes;

	int mapCount = static_cast<int>(maps.size());
	for(int i=0; i<mapCount; ++i)
	{
		for(PolyVector::const_iterator jt=maps[i].begin(), jend=maps[i].end(); jt!=jend; ++jt)
		{
			const Poly_Ptr& poly = *jt;
			int polyIndex = static_cast<int>(m_polygons->size());
			m_polygons->push_back(poly);

			// Determine the polygon's onion plane.
			OnionPlane_Ptr onionPlane(new OnionPlane(make_plane(*poly), i));

			// Try and add the onion plane to the set of unique onion planes (note that the undirected
			// form of the plane is used internally for sorting).
			int onionPlaneIndex = static_cast<int>(onionPlanes.size());
			std::pair<OnionPlaneMap::iterator,bool> kt = onionPlanes.insert(std::make_pair(onionPlane,onionPlaneIndex));

			// If this onion plane duplicates an existing one, simply add this map's index to that plane.
			if(kt.second == false)
			{
				kt.first->first->add_map_index(i);
			}

			// In either case, this polygon's onion plane index is the one returned when we tried to do the insert.
			m_polyToOnionPlaneIndex.insert(std::make_pair(polyIndex, kt.first->second));
		}
	}

	m_onionPlanes.resize(onionPlanes.size());
	for(OnionPlaneMap::const_iterator it=onionPlanes.begin(), iend=onionPlanes.end(); it!=iend; ++it)
	{
		m_onionPlanes[it->second] = it->first;
	}
}

//#################### PUBLIC METHODS ####################
template <typename Poly>
void OnionCompiler<Poly>::build_tree()
{
	int polyCount = static_cast<int>(m_polygons->size());
	std::vector<PolyIndex> polyIndices;
	polyIndices.reserve(polyCount);
	for(int i=0; i<polyCount; ++i) polyIndices.push_back(PolyIndex(i, true));

	std::vector<OnionNode_Ptr> nodes;

	boost::dynamic_bitset<> solidityDescriptor(m_mapCount);
	solidityDescriptor.set();

	build_subtree(polyIndices, nodes, solidityDescriptor);

	m_tree.reset(new OnionTree(nodes));
}

template <typename Poly>
const typename OnionCompiler<Poly>::PolyVector_Ptr&
OnionCompiler<Poly>::polygons() const
{
	return m_polygons;
}

template <typename Poly>
const OnionTree_Ptr& OnionCompiler<Poly>::tree() const
{
	return m_tree;
}

//#################### PRIVATE METHODS ####################
template <typename Poly>
OnionNode_Ptr
OnionCompiler<Poly>::build_subtree(const std::vector<PolyIndex>& polyIndices, std::vector<OnionNode_Ptr>& nodes,
								   const boost::dynamic_bitset<>& solidityDescriptor)
{
	typedef typename Poly::Vert Vert;
	typedef typename Poly::AuxData AuxData;

	OnionPlane_Ptr splitter = choose_split_plane(polyIndices);

	if(!splitter)
	{
		std::vector<int> indicesOnly;
		for(size_t i=0, size=polyIndices.size(); i<size; ++i) indicesOnly.push_back(polyIndices[i].index);
		nodes.push_back(OnionNode_Ptr(new OnionLeaf((int)nodes.size(), solidityDescriptor, indicesOnly)));
		return nodes.back();
	}

	std::vector<PolyIndex> backPolys, frontPolys;

	for(std::vector<PolyIndex>::const_iterator it=polyIndices.begin(), iend=polyIndices.end(); it!=iend; ++it)
	{
		int curIndex = it->index;
		const Poly& curPoly = *(*m_polygons)[curIndex];
		switch(classify_polygon_against_plane(curPoly, splitter->plane()))
		{
			case CP_BACK:
			{
				backPolys.push_back(*it);
				break;
			}
			case CP_COPLANAR:
			{
				if(splitter->plane().normal().dot(curPoly.normal()) > 0) frontPolys.push_back(PolyIndex(curIndex,false));
				else backPolys.push_back(PolyIndex(curIndex,false));
				break;
			}
			case CP_FRONT:
			{
				frontPolys.push_back(*it);
				break;
			}
			case CP_STRADDLE:
			{
				SplitResults<Vert,AuxData> sr = split_polygon(curPoly, splitter->plane());
				(*m_polygons)[curIndex] = sr.back;
				int k = static_cast<int>(m_polygons->size());
				m_polygons->push_back(sr.front);
				backPolys.push_back(PolyIndex(curIndex,it->splitCandidate));
				frontPolys.push_back(PolyIndex(k,it->splitCandidate));
				break;
			}
		}
	}

	boost::dynamic_bitset<> leftSolidityDescriptor = solidityDescriptor;
	boost::dynamic_bitset<> rightSolidityDescriptor = solidityDescriptor;
	const std::set<int>& mapIndices = splitter->map_indices();
	for(std::set<int>::const_iterator it=mapIndices.begin(), iend=mapIndices.end(); it!=iend; ++it)
	{
		leftSolidityDescriptor.set(*it, false);
		rightSolidityDescriptor.set(*it, true);
	}

	OnionNode_Ptr left = build_subtree(frontPolys, nodes, leftSolidityDescriptor);
	OnionNode_Ptr right = build_subtree(backPolys, nodes, rightSolidityDescriptor);

	OnionNode_Ptr subtreeRoot(new OnionBranch((int)nodes.size(), splitter, left, right));
	nodes.push_back(subtreeRoot);
	return subtreeRoot;
}

template <typename Poly>
OnionPlane_Ptr OnionCompiler<Poly>::choose_split_plane(const std::vector<PolyIndex>& polyIndices)
{
	OnionPlane_Ptr bestOnionPlane;
	double bestMetric = INT_MAX;

	int indexCount = static_cast<int>(polyIndices.size());
	for(int i=0; i<indexCount; ++i)
	{
		if(!polyIndices[i].splitCandidate) continue;

		OnionPlane_Ptr onionPlane = m_onionPlanes[m_polyToOnionPlaneIndex[polyIndices[i].index]];
		int balance = 0, splits = 0;

		for(int j=0; j<indexCount; ++j)
		{
			if(j == i) continue;

			Poly_Ptr poly = (*m_polygons)[polyIndices[j].index];
			switch(classify_polygon_against_plane(*poly, onionPlane->plane()))
			{
				case CP_BACK:
					--balance;
					break;
				case CP_COPLANAR:
					break;
				case CP_FRONT:
					++balance;
					break;
				case CP_STRADDLE:
					++splits;
					break;
			}
		}

		double metric = abs(balance) + m_weight * splits;
		if(metric < bestMetric)
		{
			bestOnionPlane = onionPlane;
			bestMetric = metric;
		}
	}

	return bestOnionPlane;
}

}
