/***
 * hesperus: Image.h
 * Copyright Stuart Golodetz, 2008. All rights reserved.
 ***/

#ifndef H_HESP_IMAGES_IMAGE
#define H_HESP_IMAGES_IMAGE

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include "PixelTypes.h"

namespace hesp {

/**
This class template allows users to create images with a pixel type of their choosing.
Several simple pixel types, such as 8-bit and 24-bit pixels, are provided as a starting
point.
*/
template <typename Pixel>
class Image
{
	//#################### TYPEDEFS ####################
protected:
	typedef shared_ptr<Image> Image_Ptr;
public:
	typedef Pixel Pixel;

	//#################### PROTECTED VARIABLES ####################
protected:
	int m_width, m_height;

	//#################### CONSTRUCTORS ####################
protected:
	Image()
	: m_width(0), m_height(0)
	{}

public:
	Image(int width, int height)
	:	m_width(width), m_height(height)
	{}

	//#################### DESTRUCTOR ####################
public:
	virtual ~Image() {}

	//#################### PUBLIC ABSTRACT OPERATORS ####################
public:
	virtual const Pixel operator()(int n) const = 0;
	virtual const Pixel operator()(int x, int y) const = 0;

	//#################### PUBLIC ABSTRACT METHODS ####################
public:
	virtual void set(int n, const Pixel& pixel) = 0;
	virtual void set(int x, int y, const Pixel& pixel) = 0;

	//#################### PUBLIC METHODS ####################
public:
	int height() const	{ return m_height; }
	int width() const	{ return m_width; }
};

//#################### TYPEDEFS ####################
typedef Image<Pixel8> Image8;
typedef Image<Pixel24> Image24;

typedef shared_ptr<Image8> Image8_Ptr;
typedef shared_ptr<const Image8> Image8_CPtr;
typedef shared_ptr<Image24> Image24_Ptr;
typedef shared_ptr<const Image24> Image24_CPtr;

}

#endif
