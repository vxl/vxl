// This is core/vil1/vil1_memory_image_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil1_memory_image_impl.h"

#include <vcl_cassert.h>
#include <vcl_cstring.h>   // strcmp() and memcpy()
#include <vcl_iostream.h>

#include <vil1/vil1_image.h>
#include <vil1/vil1_property.h>

vil1_memory_image_impl::vil1_memory_image_impl(int planes, int w, int h,
                                               vil1_memory_image_format
                                               const& format)
{
  init((void*)0, planes, w, h, format.components, format.bits_per_component,
       format.component_format);
}

vil1_memory_image_impl::vil1_memory_image_impl(int planes, int w, int h,
                                               int components,
                                               int bits_per_component,
                                               vil1_component_format component_format)
{
  init((void*)0, planes, w, h, components, bits_per_component, component_format);
}

vil1_memory_image_impl::vil1_memory_image_impl(int planes, int w, int h,
                                               vil1_pixel_format_t pixel_format)
{
  init((void*)0, planes, w, h, pixel_format);
}

vil1_memory_image_impl::vil1_memory_image_impl(int w, int h, int components,
                                               int bits_per_component,
                                               vil1_component_format component_format)
{
  init((void*)0, 1, w, h, components, bits_per_component, component_format);
}

vil1_memory_image_impl::vil1_memory_image_impl(int w, int h,
                                               vil1_pixel_format_t pixel_format)
{
  init((void*)0, 1, w, h, pixel_format);
}

vil1_memory_image_impl::vil1_memory_image_impl(vil1_memory_image_impl const& i)
  : vil1_image_impl()
{
  init((void*)0, i.planes_, i.width_, i.height_, i.components_,
       i.bits_per_component_, i.component_format_);
}

void vil1_memory_image_impl::init(void *buf,
                                  int planes, int w, int h,
                                  int components, int bits_per_component,
                                  vil1_component_format component_format)
{
  is_foreign_buf_ = (buf != 0); // true if buf is non-nil
  planes_ = planes;
  width_ = w;
  height_ = h;
  components_ = components;
  bits_per_component_ = bits_per_component;
  component_format_ = component_format;

  bytes_per_pixel_ = (bits_per_component_ * components_ + 7) / 8;
  int bytes_per_row = (width_ * bits_per_component_ * components_ + 7) / 8;

  int size = planes_ * height_ * bytes_per_row;
  if (size) {
    // non-empty image
    if (is_foreign_buf_)
      buf_ = (unsigned char *)buf;
    else
      buf_ = new unsigned char[size];
    rows_ = new void**[planes_];

    unsigned char* ptr = buf_;
    for (int p = 0; p < planes_; ++p) {
      rows_[p] = new void*[height_];
      for (int y = 0; y < height_; ++y) {
        rows_[p][y] = ptr;
        ptr += bytes_per_row;
      }
    }
  }
  else {
    // empty image
    buf_ = 0;
    rows_ = 0;
  }
}

void vil1_memory_image_impl::init(void *buf, int planes, int w, int h,
                                  vil1_pixel_format_t pixel_format)
{
  switch (pixel_format) {
  case VIL1_BYTE:     init(buf, planes, w, h, 1,  8,
                           VIL1_COMPONENT_FORMAT_UNSIGNED_INT); break;
  case VIL1_RGB_BYTE: init(buf, planes, w, h, 3,  8,
                           VIL1_COMPONENT_FORMAT_UNSIGNED_INT); break;
  case VIL1_FLOAT:    init(buf, planes, w, h, 1, 32,
                           VIL1_COMPONENT_FORMAT_UNSIGNED_INT); break;
  default:
    vcl_cerr << "vil1_memory_image_impl: crazy format!\n";
  }
}

vil1_memory_image_impl::~vil1_memory_image_impl()
{
  if (rows_) {
    for (int p = 0; p < planes_; ++p) delete[] rows_[p];
    delete [] rows_;
  }
  if (!is_foreign_buf_)
    delete [] buf_;
}


void vil1_memory_image_impl::resize(int planes, int width, int height,
                                    int components,
                                    int bits_per_component,
                                    vil1_component_format format)
{
  components_ = components;
  bits_per_component_ = bits_per_component;
  component_format_ = format;
  resize(planes, width, height);
}


void vil1_memory_image_impl::resize(int planes, int width, int height)
{
  // Resize is expensive, so only do it if the size changes.
  if (planes == planes_ && width == width_ && height == height_)
    return;

  if (rows_) {
    for (int p = 0; p < planes_; ++p) delete[] rows_[p];
    delete [] rows_;
  }
  if (!is_foreign_buf_)
    delete [] buf_;
  init((void*)0, planes, width, height, components_,
       bits_per_component_, component_format_);
}

bool vil1_memory_image_impl::get_section(void* obuf, int x0, int y0,
                                         int xs, int ys) const
{
  int bytes_per_row = (width_ * bits_per_component_ * components_ + 7) / 8;
  for (int p=0; p<planes_; ++p) {
    int offset = y0 * bytes_per_row + x0 * bytes_per_pixel_;
    if (width_ != xs) {
      int byte_out_width = bytes_per_pixel_ * xs;
      int byte_width = bytes_per_pixel_ * width_;

      for (int y = 0; y < ys; ++y) {
        vcl_memcpy((unsigned char*)obuf + y * byte_out_width, buf_ + offset + y * byte_width, byte_out_width);
      }
    } else {
      vcl_memcpy((unsigned char*)obuf, buf_ + offset, bytes_per_row * ys);
    }
    obuf = (void*)((char*)obuf + bytes_per_row*height_);
  }

  return true;
}

bool vil1_memory_image_impl::put_section(void const* ibuf, int x0,
                                         int y0, int xs, int ys)
{
  int bytes_per_row = (width_ * bits_per_component_ * components_ + 7) / 8;
  for (int p=0; p<planes_; ++p) {
    int offset = y0 * bytes_per_row + x0 * bytes_per_pixel_;
    if (width_ != xs) {
      int byte_in_width = bytes_per_pixel_ * xs;
      int byte_width = bytes_per_pixel_ * width_;

      for (int y = 0; y < ys; ++y) {
        vcl_memcpy(buf_ + offset + y * byte_width,
                   (unsigned char const*)ibuf + y*byte_in_width, byte_in_width);
      }
      ibuf = (void const*)((char const*)ibuf + byte_in_width * ys);
    } else {
      vcl_memcpy(buf_ + offset, (unsigned char const*)ibuf, bytes_per_row * ys);
      ibuf = (void const*)((char const*)ibuf + bytes_per_row * ys);
    }
  }

  return true;
}

vil1_image vil1_memory_image_impl::get_plane(unsigned int plane) const
{
  assert(plane==0);
  return const_cast<vil1_memory_image_impl*>(this);
}

bool vil1_memory_image_impl::get_property(char const *tag,
                                          void *property_value) const
{
  if (0==vcl_strcmp(tag, vil1_property_memory))
    return property_value ? (*(bool*)property_value) = true : true;

  if (0==vcl_strcmp(tag, vil1_property_top_row_first))
    return property_value ? (*(bool*)property_value) = true : true;

  if (0==vcl_strcmp(tag, vil1_property_left_first))
    return property_value ? (*(bool*)property_value) = true : true;

  return false;
}


// these functions are for creating a vil1_image from a section of
// memory that has already been created and assigned to. This is
// useful when interfacing with framegrabbers for example as no
// copying of memory is needed. On the other hand, much care must be
// taken with this method as the correct amount of memory must be
// created prior to calling these functions.
// Added by: Brendan McCane

vil1_memory_image_impl::vil1_memory_image_impl(void *buf, int planes, int w, int h,
                                               vil1_memory_image_format const& format)
{
  init(buf, planes, w, h, format.components, format.bits_per_component,
       format.component_format);
}

vil1_memory_image_impl::vil1_memory_image_impl(void *buf, int planes, int w, int h,
                                               int components, int bits_per_component,
                                               vil1_component_format component_format)
{
  init(buf, planes, w, h, components, bits_per_component, component_format);
}

vil1_memory_image_impl::vil1_memory_image_impl(void *buf, int planes, int w, int h,
                                               vil1_pixel_format_t pixel_format)
{
  init(buf, planes, w, h, pixel_format);
}

vil1_memory_image_impl::vil1_memory_image_impl(void *buf, int w, int h,
                                               int components, int bits_per_component,
                                               vil1_component_format component_format)
{
  init(buf, 1, w, h, components, bits_per_component, component_format);
}

vil1_memory_image_impl::vil1_memory_image_impl(void *buf, int w, int h,
                                               vil1_pixel_format_t pixel_format)
{
  init(buf, 1, w, h, pixel_format);
}

//: Return the name of the class;
vcl_string vil1_memory_image_impl::is_a() const
{
  static const vcl_string class_name_="vil1_memory_image_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil1_memory_image_impl::is_class(vcl_string const& s) const
{
  return s==vil1_memory_image_impl::is_a() || vil1_image_impl::is_class(s);
}
