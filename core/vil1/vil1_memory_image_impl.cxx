#ifdef __GNUC__
#pragma implementation
#endif

// This is vxl/vil/vil_memory_image_impl.cxx

#include "vil_memory_image_impl.h"

#include <vcl_cassert.h>
#include <vcl_cstring.h>   // strcmp()
#include <vcl_iostream.h>
// <memory> provides names like std::allocator and std::auto_ptr.
// it does *not* declare the memcpy() or strcmp() functions. they
// live in <cstring>
//#include <vcl_memory.h>

#include <vil/vil_image.h>

vil_memory_image_impl::vil_memory_image_impl(int planes, int w, int h,
                                             vil_memory_image_format
                                               const& format)
{
  init((void*)0, planes, w, h, format.components, format.bits_per_component,
            format.component_format);
}

vil_memory_image_impl::vil_memory_image_impl(int planes, int w, int h,
                                             int components,
                                             int bits_per_component,
                                             vil_component_format
                                                  component_format)
{
  init((void*)0, planes, w, h, components, bits_per_component,
            component_format);
}

vil_memory_image_impl::vil_memory_image_impl(int planes, int w, int h,
                                             vil_pixel_format_t pixel_format)
{
  init((void*)0, planes, w, h, pixel_format);
}

vil_memory_image_impl::vil_memory_image_impl(int w, int h, int components,
                                             int bits_per_component,
                                             vil_component_format
                                               component_format)
{
  init((void*)0, 1, w, h, components, bits_per_component, component_format);
}

vil_memory_image_impl::vil_memory_image_impl(int w, int h,
                                             vil_pixel_format_t pixel_format)
{
  init((void*)0, 1, w, h, pixel_format);
}

vil_memory_image_impl::vil_memory_image_impl
        (vil_memory_image_impl const& that)
{
  init((void*)0, that.planes_, that.width_, that.height_, that.components_,
          that.bits_per_component_, that.component_format_);
}

void vil_memory_image_impl::init(void *buf,
                                 int planes, int w, int h,
                                 int components, int bits_per_component,
                                 vil_component_format component_format)
{
  is_foreign_buf_ = (buf != 0); // true if buf is non-nil
  planes_ = planes;
  width_ = w;
  height_ = h;
  components_ = components;
  bits_per_component_ = bits_per_component;
  component_format_ = component_format;

  bytes_per_pixel_ = bits_per_component_ * components_ / 8;
  //assert(bytes_per_pixel_ * 8 == bits_per_component_ * components_);

  int size = planes_ * height_ * width_ * bytes_per_pixel_;
  if (size) {
    // non-empty image
    if (is_foreign_buf_)
      buf_ = (unsigned char *)buf;
    else
      buf_ = new unsigned char[size];
    rows_ = new void**[planes_];
    int bytes_per_row = width_ * bytes_per_pixel_;

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

void vil_memory_image_impl::init(void *buf, int planes, int w, int h,
                                 vil_pixel_format_t pixel_format)
{
  switch (pixel_format) {
  case VIL_BYTE:     init(buf, planes, w, h, 1,  8,
                            VIL_COMPONENT_FORMAT_UNSIGNED_INT); break;
  case VIL_RGB_BYTE: init(buf, planes, w, h, 3,  8,
                            VIL_COMPONENT_FORMAT_UNSIGNED_INT); break;
  case VIL_FLOAT:    init(buf, planes, w, h, 1, 32,
                            VIL_COMPONENT_FORMAT_UNSIGNED_INT); break;
  default:
    vcl_cerr << "vil_memory_image_impl: crazy format!\n";
  }
}

vil_memory_image_impl::~vil_memory_image_impl()
{
  if (rows_) {
    for (int p = 0; p < planes_; ++p) delete[] rows_[p];
    delete [] rows_;
  }
  if (!is_foreign_buf_)
    delete [] buf_;
}


void vil_memory_image_impl::resize(int planes, int width, int height,
                                   int components,
                                   int bits_per_component,
                                   vil_component_format format)
{
        components_ = components;
        bits_per_component_ = bits_per_component;
        component_format_ = format;
        resize(planes, width, height);
}


void vil_memory_image_impl::resize(int planes, int width, int height)
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

bool vil_memory_image_impl::get_section(void* obuf, int x0, int y0,
                                        int xs, int ys) const
{
  for (int p=0; p<planes_; ++p) {
    int offset = (y0 * width_ + x0) * bytes_per_pixel_;
    int byte_out_width = bytes_per_pixel_ * xs;
    if (width_ != xs) {
      int byte_width = bytes_per_pixel_ * width_;

      for (int y = 0; y < ys; ++y) {
        vcl_memcpy((unsigned char*)obuf + y * byte_out_width, buf_ + offset + y * byte_width, byte_out_width);
      }
    } else {
      vcl_memcpy((unsigned char*)obuf, buf_ + offset, byte_out_width * ys);
    }
    obuf = (void*)((char*)obuf + bytes_per_pixel_*width_*height_);
  }

  return true;
}

bool vil_memory_image_impl::put_section(void const* ibuf, int x0,
                                        int y0, int xs, int ys)
{
  for (int p=0; p<planes_; ++p) {
    int offset = (y0 * width_ + x0) * bytes_per_pixel_;
    int byte_in_width = bytes_per_pixel_ * xs;
    if (width_ != xs) {
      int byte_width = bytes_per_pixel_ * width_;

      for (int y = 0; y < ys; ++y) {
        vcl_memcpy(buf_ + offset + y * byte_width,
               (unsigned char const*)ibuf + y * byte_in_width, byte_in_width);
      }
    } else {
      vcl_memcpy(buf_ + offset, (unsigned char const*)ibuf, byte_in_width * ys);
    }
    ibuf = (void const*)((char const*)ibuf + byte_in_width * ys);
  }

  return true;
}

vil_image vil_memory_image_impl::get_plane(int /*plane*/) const
{
  assert(false);
  return 0;
}

bool vil_memory_image_impl::get_property(char const *tag,
                                         void *property_value) const
{
  if (0==vcl_strcmp(tag, "memory"))
    return true;

  return false;
}


// these functions are for creating a vil_image from a section of
// memory that has already been created and assigned to. This is
// useful when interfacing with framegrabbers for example as no
// copying of memory is needed. On the other hand, much care must be
// taken with this method as the correct amount of memory must be
// created prior to calling these functions.
// Added by: Brendan McCane

vil_memory_image_impl::vil_memory_image_impl(void *buf, int planes,
                      int w, int h, vil_memory_image_format const& format)
{
  init(buf, planes, w, h, format.components, format.bits_per_component,
        format.component_format);
}

vil_memory_image_impl::vil_memory_image_impl(void *buf, int planes, int w,
                      int h, int components, int bits_per_component,
                      vil_component_format component_format)
{
  init(buf, planes, w, h, components, bits_per_component, component_format);
}

vil_memory_image_impl::vil_memory_image_impl(void *buf, int planes, int w,
                      int h, vil_pixel_format_t pixel_format)
{
  init(buf, planes, w, h, pixel_format);
}

vil_memory_image_impl::vil_memory_image_impl(void *buf, int w, int h,
                      int components, int bits_per_component,
                      vil_component_format component_format)
{
  init(buf, 1, w, h, components, bits_per_component, component_format);
}

vil_memory_image_impl::vil_memory_image_impl(void *buf, int w, int h,
                                             vil_pixel_format_t pixel_format)
{
  init(buf, 1, w, h, pixel_format);
}

/* START_MANCHESTER_BINARY_IO_CODE */

  //: Return the name of the class;
vcl_string vil_memory_image_impl::is_a() const
{
  static const vcl_string class_name_="vil_memory_image_impl";
  return class_name_;
}

  //: Return true if the name of the class matches the argument
bool vil_memory_image_impl::is_class(vcl_string const& s) const
{
  static const vcl_string class_name_="vil_memory_image_impl";
  return s==class_name_ || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */

