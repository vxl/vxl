#ifdef __GNUC__
#pragma implementation
#endif

#include "vil_memory_image.h"

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_cstring.h> // ISO memcpy() lives in <cstring>
#include <vcl/vcl_iostream.h>

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
  :  vil_image_inhibit_derivation(0)
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(int planes, int w, int h, vil_memory_image_format const& format)
  : vil_image_inhibit_derivation(0)
  , vil_image(new vil_memory_image_impl(planes, w, h, format))
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(int planes, int w, int h, int components, int bits_per_component, vil_component_format component_format)
  : vil_image_inhibit_derivation(0)
  , vil_image(new vil_memory_image_impl(planes, w, h, components, bits_per_component, component_format))
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(int planes, int w, int h, vil_pixel_format pixel_format)
  : vil_image_inhibit_derivation(0)
  , vil_image(new vil_memory_image_impl(planes, w, h, pixel_format))
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(int w, int h, int components, int bits_per_component, vil_component_format component_format)
  : vil_image_inhibit_derivation(0)
  , vil_image(new vil_memory_image_impl(1, w, h, components, bits_per_component, component_format))
{
  cache_from_impl;
}

vil_memory_image::vil_memory_image(int w, int h, vil_pixel_format pixel_format)
  : vil_image_inhibit_derivation(0)
  , vil_image(new vil_memory_image_impl(1, w, h, pixel_format))
{
  cache_from_impl;
}

bool vil_memory_image::get_property(char const *tag, void *property_value)
{
  if (0==strcmp(tag, "memory"))
    return true;

  return false;
}

// make a memory image if input is not already one.
static
vil_image make_memory_image(vil_image const & that)
{
  if (that.get_property("memory"))
    return that;
  vcl_cerr << "copying " << that.impl() << endl;

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
  : vil_image_inhibit_derivation(0)
  , vil_image(make_memory_image(that))
{
  cache_from_impl;
  if (this->impl() != that.impl()) {
    cerr << "get_section";
    that.get_section(get_buffer(), 0, 0, width(), height());
  }
}

vil_memory_image::vil_memory_image(vil_memory_image const& that)
  : vil_image_inhibit_derivation(0)
  , vil_image(that)
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
  assert(ptr);
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  mi->resize(1, width, height);
  cache_from_impl;
}

void vil_memory_image::resize(int planes, int width, int height)
{
  assert(ptr);
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  mi->resize(planes, width, height);
  cache_from_impl;
}
