// This is core/vil1/vil1_memory_image.cxx
//:
// \file

#include <iostream>
#include "vil1_memory_image.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_impl.h>

// macro to pull down data fields from the impl structure.
#define cache_from_impl \
{ \
  if (ptr) { \
    vil1_memory_image_impl* mi = (vil1_memory_image_impl*)ptr; \
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

vil1_memory_image::vil1_memory_image()
{
  cache_from_impl;
}

vil1_memory_image::vil1_memory_image(int planes, int w, int h,
                                     vil1_memory_image_format const& format)
  : vil1_image(new vil1_memory_image_impl(planes, w, h, format))
{
  cache_from_impl;
}

vil1_memory_image::vil1_memory_image(int planes, int w, int h,
                                     int components, int bits_per_component,
                                     vil1_component_format component_format)
  : vil1_image(new vil1_memory_image_impl(planes, w, h, components, bits_per_component, component_format))
{
  cache_from_impl;
}

vil1_memory_image::vil1_memory_image(int planes, int w, int h, vil1_pixel_format_t pixel_format)
  : vil1_image(new vil1_memory_image_impl(planes, w, h, pixel_format))
{
  cache_from_impl;
}

vil1_memory_image::vil1_memory_image(int w, int h,
                                     int components, int bits_per_component,
                                     vil1_component_format component_format)
  : vil1_image(new vil1_memory_image_impl(1, w, h, components, bits_per_component, component_format))
{
  cache_from_impl;
}

vil1_memory_image::vil1_memory_image(int w,int h, vil1_pixel_format_t pixel_format)
  : vil1_image(new vil1_memory_image_impl(1, w, h, pixel_format))
{
  cache_from_impl;
}

// make a memory image if input is not already one.

vil1_image make_memory_image(vil1_image const * thatp)
{
  vil1_image const& that = *thatp;
  if (that.get_property("memory"))
    return that;
  assert(that.planes() > 0);
  assert(that.width() >= 0);
  assert(that.height() >= 0);
  assert(that.components() > 0);
  assert(that.bits_per_component() > 0);
#ifdef DEBUG
  std::cout << "copying " << that.impl() << " to a "
           << that.planes() << 'x' << that.width() << 'x' <<that.height()
           << 'x' << that.components() << " memory image, "
           << that.bits_per_component() << " bpc, component format "
           << that.component_format() << '\n' << std::flush;
#endif
  vil1_memory_image mem(that.planes(),
                        that.width(),
                        that.height(),
                        that.components(),
                        that.bits_per_component(),
                        that.component_format());
  that.get_section(mem.get_buffer(), 0, 0, that.width(), that.height());
  return mem;
}

//: If that is a memory image, just point to it, otherwise get_section
vil1_memory_image::vil1_memory_image(vil1_image const & that)
  : vil1_image(make_memory_image(&that))
{
  cache_from_impl;
}

vil1_memory_image::vil1_memory_image(vil1_memory_image const& that)
  : vil1_image(that)
{
  cache_from_impl;
}

vil1_memory_image& vil1_memory_image::operator= (vil1_memory_image const& that)
{
  vil1_image::operator= (that);
  cache_from_impl;
  return *this;
}

void vil1_memory_image::resize(int width, int height)
{
  assert(ptr!=nullptr);
  auto* mi = (vil1_memory_image_impl*)ptr;
  mi->resize(1, width, height);
  cache_from_impl;
}

void vil1_memory_image::resize(int planes, int width, int height)
{
  assert(ptr!=nullptr);
  auto* mi = (vil1_memory_image_impl*)ptr;
  mi->resize(planes, width, height);
  cache_from_impl;
}

void vil1_memory_image::assert_size(int width, int height) const
{
  if ((width != width_) || (height != height_)) {
    std::cerr << __FILE__ ": In vil1_memory_image::assert_size():\n"
             << __FILE__ ": Image has size   " << width_ << 'x' << height_ << std::endl
             << __FILE__ ": but it should be " << width  << 'x' << height  << std::endl;
    assert(false);
  }
}

// Added by Brendan McCane for creating images with already allocated
// memory. Useful for use with framegrabbers.
vil1_memory_image::vil1_memory_image(void *buf, int planes, int w, int h,
                                     vil1_memory_image_format const& format)
  : vil1_image(new vil1_memory_image_impl(buf, planes, w, h, format))
{
  cache_from_impl;
}

vil1_memory_image::vil1_memory_image(void *buf, int planes, int w, int h,
                                     int components, int bits_per_component,
                                     vil1_component_format component_format)
  : vil1_image(new vil1_memory_image_impl(buf, planes, w, h, components, bits_per_component, component_format))
{
  cache_from_impl;
}

vil1_memory_image::vil1_memory_image(void *buf, int planes, int w, int h,
                                     vil1_pixel_format_t pixel_format)
  : vil1_image(new vil1_memory_image_impl(buf, planes, w, h, pixel_format))
{
  cache_from_impl;
}

vil1_memory_image::vil1_memory_image(void *buf, int w, int h,
                                     int components, int bits_per_component,
                                     vil1_component_format component_format)
  : vil1_image(new vil1_memory_image_impl(buf, 1, w, h, components, bits_per_component, component_format))
{
  cache_from_impl;
}

vil1_memory_image::vil1_memory_image(void *buf, int w, int h,
                                     vil1_pixel_format_t pixel_format)
  : vil1_image(new vil1_memory_image_impl(buf, 1, w, h, pixel_format))
{
  cache_from_impl;
}


//: Reset this class's member variables from the image implementation
// This is useful if the impl object has been modified outside the control of this object.
void vil1_memory_image::recache_from_impl()
{
  cache_from_impl;
}
