// This is vxl/vil/vil_image_as.cxx

/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_image_as.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_string.h>

#include <vil/vil_byte.h>
#include <vil/vil_pixel.h>
#include <vil/vil_memory_image.h>
#include <vil/vil_memory_image_of_format.txx>

vil_image vil_image_as(vil_image const & img, vil_byte* dummy)
{ return vil_image_as_byte(img); }

vil_image vil_image_as(vil_image const & img, int* dummy)
{ return vil_image_as_int(img); }

vil_image vil_image_as(vil_image const & img, float* dummy)
{ return vil_image_as_float(img); }

vil_image vil_image_as(vil_image const & img, double* dummy)
{ return vil_image_as_double(img); }

vil_image vil_image_as(vil_image const & img, vil_rgb<unsigned char>* dummy)
{ return vil_image_as_rgb_byte(img); }

template <class T>
struct vil_image_as_impl : vil_image_impl, vil_memory_image_of_format<T>
{
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

/* START_MANCHESTER_BINARY_IO_CODE */

  //: Return the name of the class;
  virtual const vcl_string& is_a() const;

/* END_MANCHESTER_BINARY_IO_CODE */

};

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for int.
bool vil_image_as_impl<int>::get_section(void *buf, int x0, int y0, int width, int height) const {
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    vcl_vector<vil_byte> scan(width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i)
        static_cast<int*>(buf)[i + width*j] = int(scan[i]);
    }
    return true;
  }

  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    vcl_vector<vil_byte> scan(3*width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i) {
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
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vcl_vector<float> scan(width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i)
        static_cast<int*>(buf)[i + width*j] = int(scan[i]);
    }
    return true;
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vcl_vector<double> scan(width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i)
        static_cast<int*>(buf)[i + width*j] = int(scan[i]);
    }
    return true;
  }

  //
  else {
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
  
}

/* START_MANCHESTER_BINARY_IO_CODE */

VCL_DEFINE_SPECIALIZATION
const vcl_string& vil_image_as_impl<int>::is_a() const 
{ 
  static const vcl_string class_name_="vil_image_as_impl<int>"; 
  return class_name_; 
}

/* END_MANCHESTER_BINARY_IO_CODE */

// instantiate for int.
template struct vil_image_as_impl<int>;

vil_image vil_image_as_int(vil_image const &image) {
  return vil_image(new vil_image_as_impl<int>(image));
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for byte.
bool vil_image_as_impl<vil_byte>::get_section(void *buf, int x0, int y0, int width, int height) const {
  // byte greyscale
  switch (vil_pixel_format(image)) {
  case VIL_BYTE: {
    return image.get_section(buf, x0, y0, width, height);
  }

  // byte rgb
  case VIL_RGB_BYTE: {
    vcl_vector<vil_byte> scan(3*width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i) {
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
  case VIL_FLOAT: {
    vcl_vector<float> scan(width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i)
        static_cast<vil_byte*>(buf)[i + width*j] = vil_byte(scan[i]);
    }
    return true;
  }

  // double
  case VIL_DOUBLE: {
    vcl_vector<double> scan(width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i)
        static_cast<vil_byte*>(buf)[i + width*j] = vil_byte(scan[i]);
    }
    return true;
  }

  //
  default: {
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
  }
}
/* START_MANCHESTER_BINARY_IO_CODE */

VCL_DEFINE_SPECIALIZATION
const vcl_string& vil_image_as_impl<vil_byte>::is_a() const 
{ 
  const static vcl_string class_name_="vil_image_as_impl<vil_byte>"; 
  return class_name_; 
}

/* END_MANCHESTER_BINARY_IO_CODE */

// instantiate for byte.
template struct vil_image_as_impl<vil_byte>;

vil_image vil_image_as_byte(vil_image const &image) {
  return vil_image(new vil_image_as_impl<vil_byte>(image));
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for float.
bool vil_image_as_impl<float>::get_section(void *buf, int x0, int y0, int width, int height) const {
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    vcl_vector<vil_byte> scan(width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i)
        static_cast<float*>(buf)[i + width*j] = scan[i];
    }
    return true;
  }

  // int greyscale
  if (vil_pixel_format(image) == VIL_UINT32) {
    vcl_vector<unsigned int> scan(width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      const int SCALE = 0x01000000U;  // fixme?
      for (int i=0; i<width; ++i)
        static_cast<float*>(buf)[i + width*j] = scan[i] / SCALE;
    }
    return true;
  }

  // double greyscale
  if (vil_pixel_format(image) == VIL_DOUBLE) {
    vcl_vector<double> scan(width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i)
        static_cast<float*>(buf)[i + width*j] = scan[i];
    }
    return true;
  }

  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    vcl_vector<vil_byte> scan(3*width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i)
        static_cast<float*>(buf)[i + width*j] = 0.299*scan[3*i+0] + 0.587*scan[3*i+1] + 0.114*scan[3*i+2];
    }
    return true;
  }

  //
  else {
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
  
}

/* START_MANCHESTER_BINARY_IO_CODE */

VCL_DEFINE_SPECIALIZATION
const vcl_string& vil_image_as_impl<float>::is_a() const 
{ 
  const static vcl_string class_name_="vil_image_as_impl<float>"; 
  return class_name_; 
}

/* END_MANCHESTER_BINARY_IO_CODE */

// instantiate for float.
template struct vil_image_as_impl<float>;

vil_image vil_image_as_float(vil_image const &image) {
  return vil_image(new vil_image_as_impl<float>(image));
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for double.
bool vil_image_as_impl<double>::get_section(void *buf, int x0, int y0, int width, int height) const {
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    vcl_vector<vil_byte> scan(width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i)
        static_cast<double*>(buf)[i + width*j] = scan[i];
    }
    return true;
  }
  // float greyscale
  if (vil_pixel_format(image) == VIL_FLOAT) {
    vcl_vector<float> scan(width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i)
        static_cast<double*>(buf)[i + width*j] = scan[i];
    }
    return true;
  }

  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    vcl_vector<vil_byte> scan(3*width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i)
        static_cast<double*>(buf)[i + width*j] = 0.299*scan[3*i+0] + 0.587*scan[3*i+1] + 0.114*scan[3*i+2];
    }
    return true;
  }

  //
  else {
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
  
}


/* START_MANCHESTER_BINARY_IO_CODE */

VCL_DEFINE_SPECIALIZATION
const vcl_string& vil_image_as_impl<double>::is_a() const 
{ 
  const static vcl_string class_name_="vil_image_as_impl<double>"; 
  return class_name_; 
}

/* END_MANCHESTER_BINARY_IO_CODE */

// instantiate for double.
template struct vil_image_as_impl<double>;

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
  if (vil_pixel_format(image) == VIL_BYTE) {
    vcl_vector<vil_byte> scan(width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i) {
        static_cast<vil_byte*>(buf)[3*(i + width*j)+0] = scan[i];
        static_cast<vil_byte*>(buf)[3*(i + width*j)+1] = scan[i];
        static_cast<vil_byte*>(buf)[3*(i + width*j)+2] = scan[i];
      }
    }
    return true;
  }

  // float rgb
  else if (vil_pixel_format(image) == VIL_RGB_FLOAT) {
    vcl_vector<float> scan(3*width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i) {
        static_cast<vil_byte*>(buf)[3*(i + width*j)+0] = vil_byte(scan[3*i+0]);
        static_cast<vil_byte*>(buf)[3*(i + width*j)+1] = vil_byte(scan[3*i+1]);
        static_cast<vil_byte*>(buf)[3*(i + width*j)+2] = vil_byte(scan[3*i+2]);
      }
    }
    return true;
  }


  else if (vil_pixel_format(image) == VIL_RGBA_BYTE) {
    vcl_vector<vil_byte> scan(4*width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i) {
        static_cast<vil_byte*>(buf)[3*(i + width*j)+0] = vil_byte(scan[4*i+0]);
        static_cast<vil_byte*>(buf)[3*(i + width*j)+1] = vil_byte(scan[4*i+1]);
        static_cast<vil_byte*>(buf)[3*(i + width*j)+2] = vil_byte(scan[4*i+2]);
      }
    }
    return true;
  }



  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE)
    return image.get_section(buf, x0, y0, width, height);

  //
  else {
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
  
}
/* START_MANCHESTER_BINARY_IO_CODE */

VCL_DEFINE_SPECIALIZATION
const vcl_string& vil_image_as_impl<vil_rgb<unsigned char> >::is_a() const 
{ 
  const static vcl_string class_name_="vil_image_as_impl<vil_rgb<unsigned char> >"; 
  return class_name_; 
}

/* END_MANCHESTER_BINARY_IO_CODE */

// instantiate for vil_rgb_byte .
template struct vil_image_as_impl<vil_rgb<unsigned char> >;

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
  if (vil_pixel_format(image) == VIL_BYTE) {
    vcl_vector<vil_byte> scan(width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i) {
        static_cast<float*>(buf)[3*(i + width*j)+0] = scan[i];
        static_cast<float*>(buf)[3*(i + width*j)+1] = scan[i];
        static_cast<float*>(buf)[3*(i + width*j)+2] = scan[i];
      }
    }
    return true;
  }

  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    vcl_vector<vil_byte> scan(3*width);
    for (int j=0; j<height; ++j) {
      if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
        return false;
      for (int i=0; i<width; ++i) {
        static_cast<float*>(buf)[3*(i + width*j)+0] = scan[3*i + 0];
        static_cast<float*>(buf)[3*(i + width*j)+1] = scan[3*i + 1];
        static_cast<float*>(buf)[3*(i + width*j)+2] = scan[3*i + 2];
      }
    }
    return true;
  }

  // float rgb
  else if (vil_pixel_format(image) == VIL_RGB_FLOAT)
    return image.get_section(buf, x0, y0, width, height);

  //
  else {
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
  
}


/* START_MANCHESTER_BINARY_IO_CODE */

VCL_DEFINE_SPECIALIZATION
const vcl_string& vil_image_as_impl<vil_rgb<float> >::is_a() const 
{ 
  const static vcl_string class_name_="vil_image_as_impl<vil_rgb<float> >"; 
  return class_name_; 
}

/* END_MANCHESTER_BINARY_IO_CODE */

// instantiate for vil_rgb_float .
template struct vil_image_as_impl<vil_rgb<float> >;

vil_image vil_image_as_rgb_float(vil_image const &image) {
  return vil_image(new vil_image_as_impl<vil_rgb<float> >(image));
}


