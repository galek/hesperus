/***
 * hesperus: BrushesFileUtil.h
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#ifndef H_HESP_IO_BRUSHESFILEUTIL
#define H_HESP_IO_BRUSHESFILEUTIL

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <source/level/brushes/PolyhedralBrush.h>

namespace hesp {

struct BrushesFileUtil
{
	//#################### LOADING METHODS ####################
	template <typename Poly> static std::vector<shared_ptr<PolyhedralBrush<Poly> > > load(const std::string& filename);

	//#################### SAVING METHODS ####################
	template <typename Poly> static void save(const std::string& filename, const std::vector<shared_ptr<PolyhedralBrush<Poly> > >& brushes);
};

}

#include "BrushesFileUtil.tpp"

#endif
