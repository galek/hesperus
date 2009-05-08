/***
 * hesperus: CmpMeshMovement.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_CMPMESHMOVEMENT
#define H_HESP_CMPMESHMOVEMENT

#ifdef _MSC_VER
	#pragma warning(disable:4250)	// Disable the spurious VC++ warning about inheritance via the dominance rule: that's what I intend here
#endif

#include "CmpDirectMovement.h"
#include "ICmpMeshMovement.h"

namespace hesp {

class CmpMeshMovement : public ICmpMeshMovement, public CmpDirectMovement
{
	//#################### PRIVATE VARIABLES ####################
private:
	// The index of the nav polygon in which the centre of the entity's AABB currently resides (-1 if unknown)
	int m_curNavPolyIndex;

	// The details of the current nav link traversal (NULL if not currently traversing a link)
	Traversal_CPtr m_curTraversal;

	//#################### CONSTRUCTORS ####################
public:
	CmpMeshMovement();

	//#################### PUBLIC METHODS ####################
public:
	int cur_nav_poly_index() const;
	Traversal_CPtr cur_traversal() const;
	void set_cur_nav_poly_index(int curNavPolyIndex);
	void set_cur_traversal(const Traversal_Ptr& curTraversal);
};

}

#endif
