/***
 * hesperus: BrushesFile.tpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include <source/io/util/IOUtil.h>

namespace hesp {

//#################### LOADING METHODS ####################
/**
Loads an array of polyhedral brushes from the specified brushes file.

@param filename	The name of the brushes file
@return			The brushes
*/
template <typename Poly>
std::vector<shared_ptr<PolyhedralBrush<Poly> > > BrushesFile::load(const std::string& filename)
{
	std::ifstream is(filename.c_str());
	if(is.fail()) throw Exception("Could not open " + filename + " for reading");

	typedef PolyhedralBrush<Poly> PolyBrush;
	typedef shared_ptr<PolyBrush> PolyBrush_Ptr;
	typedef std::vector<PolyBrush_Ptr> PolyBrushVector;

	PolyBrushVector brushes;
	PolyBrush_Ptr brush;
	while(brush = IOUtil::load_polyhedral_brush<Poly>(is))
	{
		brushes.push_back(brush);
	}

	return brushes;
}

//#################### SAVING METHODS ####################
/**
Saves a brushes file to disk.

@param filename		The name of the file to which to save the brushes
@param brushes		The brushes to save
@throws Exception	If the output file could not be opened for writing
*/
template <typename Poly>
void BrushesFile::save(const std::string& filename, const std::vector<shared_ptr<PolyhedralBrush<Poly> > >& brushes)
{
	std::ofstream os(filename.c_str());
	if(os.fail()) throw Exception("Could not open " + filename + " for writing");

	int brushCount = static_cast<int>(brushes.size());
	for(int i=0; i<brushCount; ++i)
	{
		IOUtil::write_polyhedral_brush(os, *brushes[i]);
	}
}

}