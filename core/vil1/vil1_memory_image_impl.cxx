#ifdef __GNUC__
#pragma implementation
#endif

#include "vil_memory_image_impl.h"

#include <vcl_cassert.h>
#include <vcl_cstring.h>   // strcmp()
#include <vcl_iostream.h>
// <memory> provides names like std::allocator and std::auto_ptr.
// it does *not* declare the memcpy() or strcmp() functions. they
// live in <cstring>
//#include <vcl_memory.h>

#include <vil/vil_image.h>

vil_memory_image_impl::vil_memory_image_impl(int planes, int w, int h, vil_memory_image_format const& format)
{
  init(planes, w, h, format.components, format.bits_per_component, format.component_format);
}

vil_memory_image_impl::vil_memory_image_impl(int planes, int w, int h, int components, int bits_per_component, vil_component_format component_format)
{
  init(planes, w, h, components, bits_per_component, component_format);
}

vil_memory_image_impl::vil_memory_image_impl(int planes, int w, int h, vil_pixel_format pixel_format)
{
  init(planes, w, h, pixel_format);
}

vil_memory_image_impl::vil_memory_image_impl(int w, int h, int components, int bits_per_component, vil_component_format component_format)
{
  init(1, w, h, components, bits_per_component, component_format);
}

vil_memory_image_impl::vil_memory_image_impl(int w, int h, vil_pixel_format pixel_format)
{
  init(1, w, h, pixel_format);
}

vil_memory_image_impl::vil_memory_image_impl(vil_memory_image_impl const& that)
{
  init(that.planes_, that.width_, that.height_, that.components_, that.bits_per_component_, that.component_format_);
}

void vil_memory_image_impl::init(int planes, int w, int h, 
				 int components, int bits_per_component, 
				 vil_component_format component_format)
{
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

void vil_memory_image_impl::init(int planes, int w, int h, vil_pixel_format pixel_format)
{
  switch (pixel_format) {
  case VIL_BYTE:     init(planes, w, h, 1,  8, VIL_COMPONENT_FORMAT_UNSIGNED_INT); break;
  case VIL_RGB_BYTE: init(planes, w, h, 3,  8, VIL_COMPONENT_FORMAT_UNSIGNED_INT); break;
  case VIL_FLOAT:    init(planes, w, h, 1, 32, VIL_COMPONENT_FORMAT_UNSIGNED_INT); break;
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
  delete [] buf_;
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
  delete [] buf_;
  init(planes, width, height, components_, bits_per_component_, component_format_);
}

bool vil_memory_image_impl::get_section(void* obuf, int x0, int y0, int xs, int ys) const
{
  for (int p=0; p<planes_; ++p) {
    int offset = (y0 * width_ + x0) * bytes_per_pixel_;
    int byte_out_width = bytes_per_pixel_ * xs;
    if (width_ != xs) {
      int byte_width = bytes_per_pixel_ * width_;
      
      for (int y = 0; y < ys; ++y) {
	memcpy((unsigned char*)obuf + y * byte_out_width, buf_ + offset + y * byte_width, byte_out_width);
      }
    } else {
      memcpy((unsigned char*)obuf, buf_ + offset, byte_out_width * ys);
    }
    obuf = (void*)((char*)obuf + bytes_per_pixel_*width_*height_);
  }

  return true;
}

bool vil_memory_image_impl::put_section(void const* ibuf, int x0, int y0, int xs, int ys)
{
  for (int p=0; p<planes_; ++p) {
    int offset = (y0 * width_ + x0) * bytes_per_pixel_;
    int byte_in_width = bytes_per_pixel_ * xs;
    if (width_ != xs) {
      int byte_width = bytes_per_pixel_ * width_;
    
      for (int y = 0; y < ys; ++y) {
	memcpy(buf_ + offset + y * byte_width, (unsigned char const*)ibuf + y * byte_in_width, byte_in_width);
      }
    } else {
      memcpy(buf_ + offset, (unsigned char const*)ibuf, byte_in_width * ys);
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

bool vil_memory_image_impl::get_property(char const *tag, void *property_value) const
{
  if (0==strcmp(tag, "memory"))
    return true;

  return false;
}
