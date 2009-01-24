/***
 * hesperus: FileSectionUtil_Load.tpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

namespace hesp {

//#################### LOADING METHODS ####################
/**
Loads a polygon section from the specified std::istream.

@param is			The std::istream
@param sectionName	The name of the section in the file (e.g. Polygons, Portals, etc.)
@param polygons		The array into which to read the polygons
*/
template <typename Poly>
void FileSectionUtil::load_polygons_section(std::istream& is, const std::string& sectionName, std::vector<shared_ptr<Poly> >& polygons)
{
	LineIO::read_checked_line(is, sectionName);
	LineIO::read_checked_line(is, "{");
	load_counted_polygons(is, polygons);
	LineIO::read_checked_line(is, "}");
}

//#################### LOADING SUPPORT METHODS ####################
/**
Loads a sequence of polygons of known length from a std::istream, one per line.
The polygon count is stored on a separate line before the polygons.

@param is		The std::istream
@param polygons	The std::vector into which to write the loaded polygons
*/
template <typename Poly>
void FileSectionUtil::load_counted_polygons(std::istream& is, std::vector<shared_ptr<Poly> >& polygons)
{
	std::string line;

	LineIO::read_line(is, line, "polygon count");
	try
	{
		int polyCount = boost::lexical_cast<int,std::string>(line);
		load_polygons(is, polygons, polyCount);
	}
	catch(boost::bad_lexical_cast&) { throw Exception("The polygon count is not an integer"); }
}

/**
Loads a polygon from a std::string (generally a line of text taken from a file).

@param line	The std::string containing the polygon definition
@param n	The number of the line in the file (if available), as a std::string
@return		The polygon
*/
template <typename Vert, typename AuxData>
shared_ptr<Polygon<Vert,AuxData> > FileSectionUtil::load_polygon(const std::string& line, const std::string& n)
{
	typedef Polygon<Vert,AuxData> Poly;
	typedef shared_ptr<Poly> Poly_Ptr;

	// Read the vertex count.
	std::string::size_type L = line.find(' ');
	if(L == std::string::npos) throw Exception("Bad input on line " + n);
	std::string vertCountString = line.substr(0,L);
	int vertCount;
	try								{ vertCount = boost::lexical_cast<int,std::string>(vertCountString); }
	catch(boost::bad_lexical_cast&)	{ throw Exception("Bad vertex count on line " + n); }

	// Read the auxiliary data.
	std::string::size_type R = line.find_last_of(')');
	if(R == std::string::npos || R+2 >= line.length()) throw Exception("Bad input on line " + n);
	std::string auxDataString = line.substr(R+2);
	boost::trim(auxDataString);
	AuxData auxData;
	try								{ auxData = boost::lexical_cast<AuxData,std::string>(auxDataString); }
	catch(boost::bad_lexical_cast&)	{ throw Exception("Bad auxiliary data on line " + n); }

	// Read the vertices.
	std::string verticesString = line.substr(L+1, R-L);
	std::vector<Vert> vertices;

	typedef boost::char_separator<char> sep;
	typedef boost::tokenizer<sep> tokenizer;
	tokenizer tok(verticesString.begin(), verticesString.end(), sep(" "));
	std::vector<std::string> tokens(tok.begin(), tok.end());
	int tokensPerVert = static_cast<int>(tokens.size()) / vertCount;
	if(tokensPerVert < 3) throw Exception("Bad vertex data on line " + n);

	for(int i=0; i<vertCount; ++i)
	{
		int offset = i*tokensPerVert;
		if(tokens[offset] != "(" || tokens[offset+tokensPerVert-1] != ")") throw Exception("Bad vertex data on line " + n);

		std::vector<std::string> components(&tokens[offset+1], &tokens[offset+tokensPerVert-1]);
		try					{ vertices.push_back(Vert(components)); }
		catch(Exception& e)	{ throw Exception(e.cause()); }
	}

	return Poly_Ptr(new Poly(vertices, auxData));
}

/**
Loads a sequence of polygons from a stream, one per line.

@param is		The stream from which to load
@param polygons	The std::vector into which to write the loaded polygons
*/
template <typename Vert, typename AuxData>
void FileSectionUtil::load_polygons(std::istream& is, std::vector<shared_ptr<Polygon<Vert,AuxData> > >& polygons, int maxToRead)
{
	typedef Polygon<Vert,AuxData> Poly;
	typedef shared_ptr<Poly> Poly_Ptr;

	std::string line;
	int n = 1;
	while(std::getline(is, line))
	{
		boost::trim(line);
		if(line != "")
		{
			polygons.push_back(load_polygon<Vert,AuxData>(line, boost::lexical_cast<std::string,int>(n)));
		}

		++n;
		if(n > maxToRead) break;
	}
}

/**
Loads a polyhedral brush from a std::istream.

@param is			The std::istream
@return				The polyhedral brush, or NULL if EOF was encountered
@throws Exception	If anything else goes wrong whilst trying to read the polyhedral brush
*/
template <typename Poly>
shared_ptr<PolyhedralBrush<Poly> > FileSectionUtil::load_polyhedral_brush(std::istream& is)
{
	typedef PolyhedralBrush<Poly> PolyBrush;
	typedef shared_ptr<PolyBrush> PolyBrush_Ptr;

	std::string line;

	if(!std::getline(is,line)) return PolyBrush_Ptr();
	if(line != "{") throw Exception("Expected {");

	// Read bounds.
	LineIO::read_line(is, line, "bounds");
	AABB3d bounds = read_aabb<Vector3d>(line);

	// Read faces.
	typedef shared_ptr<Poly> Poly_Ptr;
	std::vector<Poly_Ptr> faces;
	FileSectionUtil::load_counted_polygons(is, faces);

	LineIO::read_checked_line(is, "}");

	return PolyBrush_Ptr(new PolyBrush(bounds, faces));
}

/**
Loads a sequence of polygons of unknown length from a std::istream, one per line.

@param is		The std::istream
@param polygons	The std::vector into which to write the loaded polygons
*/
template <typename Poly>
void FileSectionUtil::load_uncounted_polygons(std::istream& is, std::vector<shared_ptr<Poly> >& polygons)
{
	return load_polygons(is, polygons);
}

}
