/***
 * hesperus: VisTable.h
 * Copyright Stuart Golodetz, 2008. All rights reserved.
 ***/

#ifndef H_HESP_LEVEL_VIS_VISTABLE
#define H_HESP_LEVEL_VIS_VISTABLE

#include <vector>

namespace hesp {

//#################### ENUMERATIONS ####################
// Note:	We don't use bool instead of LeafVisState because VisTable
//			can be implemented internally using std::vector, and sadly
//			std::vector<bool> is not a container (yuk!)
enum LeafVisState
{
	LEAFVIS_NO,
	LEAFVIS_YES
};

//#################### CLASSES ####################
/**
This class template represents a visibility table. It stores the
visibility relation for an ordered set of objects (note that for
the purposes of the PVS calculator, it can store the visibility
relation for both portals and leaves).
*/
template <typename T>
class VisTable
{
	//#################### PRIVATE VARIABLES ####################
private:
	std::vector<std::vector<T> > m_table;

	//#################### CONSTRUCTORS ####################
public:
	VisTable(int n, const T& initialValue = T());

	//#################### PUBLIC OPERATORS ####################
public:
	T& operator()(int i, int j);
	const T& operator()(int i, int j) const;

	//#################### PUBLIC METHODS ####################
public:
	int size() const;
};

//#################### TYPEDEFS ####################
typedef VisTable<LeafVisState> LeafVisTable;
typedef shared_ptr<LeafVisTable> LeafVisTable_Ptr;

}

#include "VisTable.tpp"

#endif