// This is ./vxl/vil/vil_memory_image.cxx
#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file

#include "vil_memory_image.h"

#include <vcl_cassert.h>
#include <vcl_cstring.h> // ISO memcpy() lives in <cstring>
#include <vcl_iostream.h>

#include <vil/vil_image.h>
#include <vil/vil_memory_image_impl.h>

// macro to pull down data fields from the impl structure.
#define cache_from_impl \
{ \
  if (ptr) { \
    vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr; \
    this->width_  = mi->width_; \
    this->height_ = mi->height_; \
    this->rows0_  = mi->rows_ ? mi->rows_[0] : 0; \
  } \
  else { \
    this->width_  = 0; \
    this->height_ = 0; \
    this->rows0_  = 0; \
  } \
}

vil_memory_image::vil_memory_image()
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(int planes, int w, int h,
                                   vil_memory_image_format const& format)
  : vil_image(new vil_memory_image_impl(planes, w, h, format))
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(int planes, int w, int h,
                                   int components, int bits_per_component,
                                   vil_component_format component_format)
  : vil_image(new vil_memory_image_impl(planes, w, h, components, bits_per_component, component_format))
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(int planes, int w, int h, vil_pixel_format_t pixel_format)
  : vil_image(new vil_memory_image_impl(planes, w, h, pixel_format))
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(int w, int h,
                                   int components, int bits_per_component,
                                   vil_component_format component_format)
  : vil_image(new vil_memory_image_impl(1, w, h, components, bits_per_component, component_format))
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(int w,int h, vil_pixel_format_t pixel_format)
  : vil_image(new vil_memory_image_impl(1, w, h, pixel_format))
{
  cache_from_impl;
}

// make a memory image if input is not already one.

vil_image make_memory_image(vil_image const * thatp)
{
  vil_image const& that = *thatp;
  //noblather vcl_cerr << thatp << " ptr ";
  //noblather vcl_cerr << that.impl() << "  ";
  if (that.get_property("memory"))
    return that;
  //vcl_cerr << "copying " << that.impl() << endl;
  vil_memory_image mem(that.planes(),
                       that.width(),
                       that.height(),
                       that.components(),
                       that.bits_per_component(),
                       that.component_format());
  that.get_section(mem.get_buffer(), 0, 0, that.width(), that.height());
  return mem;
}

//: If that is a memory image, just point to it, otherwise get_section
vil_memory_image::vil_memory_image(vil_image const & that)
  : vil_image(make_memory_image(&that))
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(vil_memory_image const& that)
  : vil_image(that)
{
  cache_from_impl;
}

vil_memory_image& vil_memory_image::operator= (vil_memory_image const& that)
{
  vil_image::operator= (that);
  cache_from_impl;
  return *this;
}

void vil_memory_image::resize(int width, int height)
{
  assert(ptr!=0);
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  mi->resize(1, width, height);
  cache_from_impl;
}

void vil_memory_image::resize(int planes, int width, int height)
{
  assert(ptr!=0);
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  mi->resize(planes, width, height);
  cache_from_impl;
}

void vil_memory_image::assert_size(int width, int height) const
{
  if ((width != width_) || (height != height_)) {
    vcl_cerr << __FILE__ ": In vil_memory_image::assert_size():\n";
    vcl_cerr << __FILE__ ": Image has size   " << width_ << 'x' << height_ << vcl_endl;
    vcl_cerr << __FILE__ ": but it should be " << width  << 'x' << height  << vcl_endl;
    assert(false);
  }
}

// Added by Brendan McCane for creating images with already allocated
// memory. Useful for use with framegrabbers.
vil_memory_image::vil_memory_image(void *buf, int planes, int w, int h,
                                   vil_memory_image_format const& format)
  : vil_image(new vil_memory_image_impl(buf, planes, w, h, format))
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(void *buf, int planes, int w, int h,
                                   int components, int bits_per_component,
                                   vil_component_format component_format)
  : vil_image(new vil_memory_image_impl(buf, planes, w, h, components, bits_per_component, component_format))
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(void *buf, int planes, int w, int h,
                                   vil_pixel_format_t pixel_format)
  : vil_image(new vil_memory_image_impl(buf, planes, w, h, pixel_format))
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(void *buf, int w, int h,
                                   int components, int bits_per_component,
                                   vil_component_format component_format)
  : vil_image(new vil_memory_image_impl(buf, 1, w, h, components, bits_per_component, component_format))
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(void *buf, int w, int h,
                                   vil_pixel_format_t pixel_format)
  : vil_image(new vil_memory_image_impl(buf, 1, w, h, pixel_format))
{
  cache_from_impl;
}


//: Reset this class's member variables from the image implementation
// This is is useful if the impl object has been modified outside the control of this object.
void vil_memory_image::recache_from_impl()
{
  cache_from_impl;
}
