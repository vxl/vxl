#ifdef __GNUC__
#pragma implementation "vil_memory_image.h"
#endif

#include "vil_memory_image.h"

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_memory.h>

#include <vil/vil_image.h>
#include <vil/vil_memory_image_impl.h>

vil_memory_image::vil_memory_image(int planes, int w, int h, vil_memory_image_format const& format)
  : vil_image(new vil_memory_image_impl(planes, w, h, format))
{
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  this->width_ = mi->width_;
  this->height_ = mi->height_;
  this->rows0_ = mi->rows0_;
}

vil_memory_image::vil_memory_image(int planes, int w, int h, int components, int bits_per_component, vil_component_format component_format)
  : vil_image(new vil_memory_image_impl(planes, w, h, components, bits_per_component, component_format))
{
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  this->width_ = mi->width_;
  this->height_ = mi->height_;
  this->rows0_ = mi->rows0_;
}

vil_memory_image::vil_memory_image(int planes, int w, int h, vil_pixel_format pixel_format)
  : vil_image(new vil_memory_image_impl(planes, w, h, pixel_format))
{
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  this->width_ = mi->width_;
  this->height_ = mi->height_;
  this->rows0_ = mi->rows0_;
}

vil_memory_image::vil_memory_image(int w, int h, int components, int bits_per_component, vil_component_format component_format)
  : vil_image(new vil_memory_image_impl(1, w, h, components, bits_per_component, component_format))
{
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  this->width_ = mi->width_;
  this->height_ = mi->height_;
  this->rows0_ = mi->rows0_;
}

vil_memory_image::vil_memory_image(int w, int h, vil_pixel_format pixel_format)
  : vil_image(new vil_memory_image_impl(1, w, h, pixel_format))
{
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  this->width_ = mi->width_;
  this->height_ = mi->height_;
  this->rows0_ = mi->rows0_;
}

vil_memory_image::vil_memory_image(vil_image const & that, char const * /*silly_hack*/)
  : vil_image(new vil_memory_image_impl(that.planes(), 
					that.width(), 
					that.height(),
					that.components(),
					that.bits_per_component(), 
					that.component_format()))
{
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  this->width_ = mi->width_;
  this->height_ = mi->height_;
  this->rows0_ = mi->rows0_;

  that.get_section(get_buffer(), 0, 0, width(), height());
}

vil_memory_image::vil_memory_image(vil_memory_image const& that)
  : vil_image(that)
{
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  this->width_ = mi->width_;
  this->height_ = mi->height_;
  this->rows0_ = mi->rows0_;
}

vil_memory_image& vil_memory_image::operator= (vil_memory_image const& that)
{
  vil_image::operator= (that);
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  this->width_ = mi->width_;
  this->height_ = mi->height_;
  this->rows0_ = mi->rows0_;
  return *this;
}

void vil_memory_image::resize(int width, int height)
{
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  mi->resize(width, height);
  this->width_ = mi->width_;
  this->height_ = mi->height_;
  this->rows0_ = mi->rows0_;
}

void vil_memory_image::resize(int planes, int width, int height)
{
  vil_memory_image_impl* mi = (vil_memory_image_impl*)ptr;
  mi->resize(planes, width, height);
  this->width_ = mi->width_;
  this->height_ = mi->height_;
  this->rows0_ = mi->rows0_;
}
