/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_image_as.h"

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>

#include <vil/vil_byte.h>
#include <vil/vil_pixel.h>
#include <vil/vil_buffer.h>
#include <vil/vil_memory_image.h>
#include <vil/vil_memory_image_of_format.txx>

template <class T>
class vil_image_as_impl : public vil_image_impl, public vil_memory_image_of_format<T> {
public:
  typedef vil_memory_image_of_format<T> format;
  vil_image image;
  vil_image_as_impl(vil_image const &underlying) : image(underlying) { }
  int planes() const { return 1; }
  int width() const { return image.width(); }
  int height() const { return image.height(); }
  int components() const { return format::components; }
  int bits_per_component() const { return format::bits_per_component; }
  vil_component_format component_format() const { return format::component_format; }
  vil_image get_plane(int i) const { assert(i==0); return const_cast<vil_image_as_impl*>(this); }

  //char const* file_format() const { return 0; }
  //bool get_property(char const* tag, void* property_value_out = 0) const { return false; }

  // There is no default implementation of this method. It must be 
  // specialized by hand for each T.
  bool get_section(void *buf, int x0, int y0, int width, int height) const;

  // This always fails, even if the underlying image is an image-of-T
  bool put_section(void const *, int, int, int, int) { return false; }
};

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for int.
bool vil_image_as_impl<int>::get_section(void *buf, int x0, int y0, int width, int height) const {
  // byte greyscale
  if (vil_pixel_type(image) == VIL_BYTE) {
    vil_buffer<vil_byte> scan(width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i)
	static_cast<int*>(buf)[i + width*j] = int(scan[i]);
    }
    return true;
  }
  
  // byte rgb
  else if (vil_pixel_type(image) == VIL_RGB_BYTE) {
    vil_buffer<vil_byte> scan(3*width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i) {
	// use different weights?
        unsigned char r(scan[3*i+0]);
        unsigned char g(scan[3*i+1]);
        unsigned char b(scan[3*i+2]);
	static_cast<int*>(buf)[i + width*j] = int(0.5+r*0.299+0.587*g+0.114*b);
      }
    }
    return true;
  }

  // float
  else if (vil_pixel_type(image) == VIL_FLOAT) {
    vil_buffer<float> scan(width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i)
	static_cast<int*>(buf)[i + width*j] = int(scan[i]);
    }
    return true;
  }
  
  // double
  else if (vil_pixel_type(image) == VIL_DOUBLE) {
    vil_buffer<double> scan(width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i)
	static_cast<int*>(buf)[i + width*j] = int(scan[i]);
    }
    return true;
  }
  
  //
  else {
    cerr << __FILE__ ": get_section() not implemented for " << image << endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
  
}
// instantiate for int.
  template class vil_image_as_impl<int>;

vil_image vil_image_as_int(vil_image const &image) {
  return vil_image(new vil_image_as_impl<int>(image));
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for byte.
bool vil_image_as_impl<vil_byte>::get_section(void *buf, int x0, int y0, int width, int height) const {
  // byte greyscale
  if (vil_pixel_type(image) == VIL_BYTE)
    return image.get_section(buf, x0, y0, width, height);
  
  // byte rgb
  else if (vil_pixel_type(image) == VIL_RGB_BYTE) {
    vil_buffer<vil_byte> scan(3*width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i) {
	// use different weights?
        unsigned char r(scan[3*i+0]);
        unsigned char g(scan[3*i+1]);
        unsigned char b(scan[3*i+2]);
	static_cast<vil_byte*>(buf)[i + width*j] = unsigned(0.5+r*0.299+0.587*g+0.114*b);
      }
    }
    return true;
  }

  // float
  else if (vil_pixel_type(image) == VIL_FLOAT) {
    vil_buffer<float> scan(width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i)
	static_cast<vil_byte*>(buf)[i + width*j] = vil_byte(scan[i]);
    }
    return true;
  }
  // double
  else if (vil_pixel_type(image) == VIL_DOUBLE) {
    vil_buffer<double> scan(width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i)
	static_cast<vil_byte*>(buf)[i + width*j] = vil_byte(scan[i]);
    }
    return true;
  }
  
  //
  else {
    cerr << __FILE__ ": get_section() not implemented for " << image << endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
  
}
// instantiate for byte.
template class vil_image_as_impl<vil_byte>;

vil_image vil_image_as_byte(vil_image const &image) {
  return vil_image(new vil_image_as_impl<vil_byte>(image));
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for float.
bool vil_image_as_impl<float>::get_section(void *buf, int x0, int y0, int width, int height) const {
  // byte greyscale
  if (vil_pixel_type(image) == VIL_BYTE) {
    vil_buffer<vil_byte> scan(width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i)
	static_cast<float*>(buf)[i + width*j] = scan[i];
    }
    return true;
  }
  // double greyscale
  if (vil_pixel_type(image) == VIL_DOUBLE) {
    vil_buffer<double> scan(width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i)
	static_cast<float*>(buf)[i + width*j] = scan[i];
    }
    return true;
  }
  
  // byte rgb
  else if (vil_pixel_type(image) == VIL_RGB_BYTE) {
    vil_buffer<vil_byte> scan(3*width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i)
	static_cast<float*>(buf)[i + width*j] = 0.299*scan[3*i+0] + 0.587*scan[3*i+1] + 0.114*scan[3*i+2];
    }
    return true;
  }
  
  //
  else {
    cerr << __FILE__ ": get_section() not implemented for " << image << endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
  
}
// instantiate for float.
template class vil_image_as_impl<float>;

vil_image vil_image_as_float(vil_image const &image) {
  return vil_image(new vil_image_as_impl<float>(image));
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for double.
bool vil_image_as_impl<double>::get_section(void *buf, int x0, int y0, int width, int height) const {
  // byte greyscale
  if (vil_pixel_type(image) == VIL_BYTE) {
    vil_buffer<vil_byte> scan(width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i)
	static_cast<double*>(buf)[i + width*j] = scan[i];
    }
    return true;
  }
  // float greyscale
  if (vil_pixel_type(image) == VIL_FLOAT) {
    vil_buffer<float> scan(width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i)
	static_cast<double*>(buf)[i + width*j] = scan[i];
    }
    return true;
  }
  
  // byte rgb
  else if (vil_pixel_type(image) == VIL_RGB_BYTE) {
    vil_buffer<vil_byte> scan(3*width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i)
	static_cast<double*>(buf)[i + width*j] = 0.299*scan[3*i+0] + 0.587*scan[3*i+1] + 0.114*scan[3*i+2];
    }
    return true;
  }
  
  //
  else {
    cerr << __FILE__ ": get_section() not implemented for " << image << endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
  
}
// instantiate for double.
template class vil_image_as_impl<double>;

vil_image vil_image_as_double(vil_image const &image) {
  return vil_image(new vil_image_as_impl<double>(image));
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for rgb.
bool vil_image_as_impl<vil_rgb<unsigned char> >::get_section(void *buf, 
							     int x0, int y0, 
							     int width, int height) const 
{
  // byte greyscale
  if (vil_pixel_type(image) == VIL_BYTE) {
    vil_buffer<vil_byte> scan(width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i) {
	static_cast<vil_byte*>(buf)[3*(i + width*j)+0] = scan[i];
	static_cast<vil_byte*>(buf)[3*(i + width*j)+1] = scan[i];
	static_cast<vil_byte*>(buf)[3*(i + width*j)+2] = scan[i];
      }
    }
    return true;
  }

  // float rgb
  else if (vil_pixel_type(image) == VIL_RGB_FLOAT) {
    vil_buffer<float> scan(3*width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i) {
	static_cast<vil_byte*>(buf)[3*(i + width*j)+0] = vil_byte(scan[3*i+0]);
	static_cast<vil_byte*>(buf)[3*(i + width*j)+1] = vil_byte(scan[3*i+1]);
	static_cast<vil_byte*>(buf)[3*(i + width*j)+2] = vil_byte(scan[3*i+2]);
      }
    }
    return true;
  }

  // byte rgb
  else if (vil_pixel_type(image) == VIL_RGB_BYTE)
    return image.get_section(buf, x0, y0, width, height);
  
  //
  else {
    cerr << __FILE__ ": get_section() not implemented for " << image << endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
  
}
// instantiate for vil_rgb_byte .
template class vil_image_as_impl<vil_rgb<unsigned char> >;

vil_image vil_image_as_rgb_byte(vil_image const &image) {
  return vil_image(new vil_image_as_impl<vil_rgb<unsigned char> >(image));
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for rgb.
bool vil_image_as_impl<vil_rgb<float> >::get_section(void *buf, 
						     int x0, int y0, 
						     int width, int height) const 
{
  // byte greyscale
  if (vil_pixel_type(image) == VIL_BYTE) {
    vil_buffer<vil_byte> scan(width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i) {
	static_cast<float*>(buf)[3*(i + width*j)+0] = scan[i];
	static_cast<float*>(buf)[3*(i + width*j)+1] = scan[i];
	static_cast<float*>(buf)[3*(i + width*j)+2] = scan[i];
      }
    }
    return true;
  }
  
  else if (vil_pixel_type(image) == VIL_RGB_BYTE) {
    vil_buffer<vil_byte> scan(3*width);
    for (unsigned j=0; j<height; ++j) {
      if (!image.get_section(scan.data(), x0, y0+j, width, 1))
	return false;
      for (unsigned i=0; i<width; ++i) {
	static_cast<float*>(buf)[3*(i + width*j)+0] = scan[3*i + 0];
	static_cast<float*>(buf)[3*(i + width*j)+1] = scan[3*i + 1];
	static_cast<float*>(buf)[3*(i + width*j)+2] = scan[3*i + 2];
      }
    }
    return true;    
  }
    
  // float rgb
  else if (vil_pixel_type(image) == VIL_RGB_FLOAT)
    return image.get_section(buf, x0, y0, width, height);
  
  //
  else {
    cerr << __FILE__ ": get_section() not implemented for " << image << endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
  
}
// instantiate for vil_rgb_float .
template class vil_image_as_impl<vil_rgb<float> >;

vil_image vil_image_as_rgb_float(vil_image const &image) {
  return vil_image(new vil_image_as_impl<vil_rgb<float> >(image));
}

