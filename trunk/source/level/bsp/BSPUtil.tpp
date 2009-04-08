/***
 * hesperus: BSPUtil.tpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include <source/math/geom/GeomUtil.h>

namespace hesp {

//#################### PUBLIC METHODS ####################
template <typename Vert, typename AuxData>
std::list<int> BSPUtil::find_leaf_indices(const Polygon<Vert,AuxData>& poly, const BSPTree_Ptr& tree)
{
	return find_leaf_indices(poly, tree->root());
}

//#################### PRIVATE METHODS ####################
template <typename Vert, typename AuxData>
std::list<int> BSPUtil::find_leaf_indices(const Polygon<Vert,AuxData>& poly, const BSPNode_Ptr& node)
{
	if(node->is_leaf())
	{
		const BSPLeaf *leaf = node->as_leaf();
		std::list<int> leafIndices;
		leafIndices.push_back(leaf->leaf_index());
		return leafIndices;
	}
	else
	{
		const BSPBranch *branch = node->as_branch();
		switch(classify_polygon_against_plane(poly, *branch->splitter()))
		{
			case CP_BACK:
			{
				return find_leaf_indices(poly, branch->right());
			}
			case CP_COPLANAR:
			case CP_FRONT:
			{
				return find_leaf_indices(poly, branch->left());
			}
			default:	// case CP_STRADDLE
			{
				SplitResults<Vert,AuxData> sr = split_polygon(poly, *branch->splitter());
				std::list<int> leafIndices;
				leafIndices.splice(leafIndices.end(), find_leaf_indices(*sr.front, branch->left()));
				leafIndices.splice(leafIndices.end(), find_leaf_indices(*sr.back, branch->right()));
				return leafIndices;
			}
		}
	}
}

}
