// This is vxl/vil/vil_image_as.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vil_image_as.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_string.h>

#include <vil/vil_byte.h>
#include <vil/vil_pixel.h>
#include <vil/vil_memory_image_of_format.txx>

#include <vxl_config.h>


template <class T>
struct vil_image_as_impl : public vil_image_impl, public vil_memory_image_of_format<T>
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

#if 0
  char const* file_format() const { return 0; }
  bool get_property(char const* tag, void* property_value_out = 0) const { return false; }
#endif

  // There is no default implementation of this method. It must be
  // specialized by hand for each T.
  bool get_section(void *buf, int x0, int y0, int width, int height) const;

  // This always fails, even if the underlying image is an image-of-T
  bool put_section(void const *, int, int, int, int) { return false; }

  //: Return the name of the class;
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;
};

//--------------------------------------------------------------------------------

template<class Inp, class Out>
bool convert_grey_to_grey( const vil_image& image, void* buf, int x0, int y0, int width, int height, Inp* , Out* )
{
  vcl_vector<Inp> scan(width);
  for (int j=0; j<height; ++j) {
    if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
      return false;
    for (int i=0; i<width; ++i)
      static_cast<Out*>(buf)[i + width*j] = Out(scan[i]);
  }
  return true;
}


template<class Inp, class Out>
bool convert_rgb_to_grey( const vil_image& image, void* buf, int x0, int y0, int width, int height, Inp* , Out* )
{
  vcl_vector<Inp> scan(3*width);
  for (int j=0; j<height; ++j) {
    if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
      return false;
    for (int i=0; i<width; ++i) {
      Inp r(scan[3*i+0]);
      Inp g(scan[3*i+1]);
      Inp b(scan[3*i+2]);
      // Weights convert from linear RGB to CIE luminance assuming a
      // modern monitor.  See Charles Pontyon's Colour FAQ
      // http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html
      static_cast<Out*>(buf)[i + width*j] = Out(0.2125*r+0.7154*g+0.072*b);
      // This are the old NTSC weights.
      //static_cast<Out*>(buf)[i + width*j] = Out(0.5+r*0.299+0.587*g+0.114*b);
    }
  }
  return true;
}


template<class Inp, class Out>
bool convert_grey_to_rgb( const vil_image& image, void* buf, int x0, int y0, int width, int height, Inp* , Out* )
{
  vcl_vector<Inp> scan(width);
  for (int j=0; j<height; ++j) {
    if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
      return false;
    for (int i=0; i<width; ++i) {
      static_cast<Out*>(buf)[3*(i + width*j)+0] = Out(scan[i]);
      static_cast<Out*>(buf)[3*(i + width*j)+1] = Out(scan[i]);
      static_cast<Out*>(buf)[3*(i + width*j)+2] = Out(scan[i]);
    }
  }
  return true;
}


template<class Inp, class Out>
bool convert_rgb_to_rgb( const vil_image& image, void* buf, int x0, int y0, int width, int height, Inp* , Out* )
{
  vcl_vector<Inp> scan(3*width);
  for (int j=0; j<height; ++j) {
    if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
      return false;
    for (int i=0; i<width; ++i) {
      static_cast<Out*>(buf)[3*(i + width*j)+0] = Out(scan[3*i + 0]);
      static_cast<Out*>(buf)[3*(i + width*j)+1] = Out(scan[3*i + 1]);
      static_cast<Out*>(buf)[3*(i + width*j)+2] = Out(scan[3*i + 2]);
    }
  }
  return true;
}

template<class Inp, class Out>
bool convert_rgba_to_rgb( const vil_image& image, void* buf, int x0, int y0, int width, int height, Inp* , Out* )
{
  vcl_vector<Inp> scan(4*width);
  for (int j=0; j<height; ++j) {
    if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
      return false;
    for (int i=0; i<width; ++i) {
      static_cast<Out*>(buf)[3*(i + width*j)+0] = Out(scan[4*i + 0]);
      static_cast<Out*>(buf)[3*(i + width*j)+1] = Out(scan[4*i + 1]);
      static_cast<Out*>(buf)[3*(i + width*j)+2] = Out(scan[4*i + 2]);
    }
  }
  return true;
}


template<class Inp, class Out>
bool convert_rgba_to_grey( const vil_image& image, void* buf, int x0, int y0, int width, int height, Inp* , Out* )
{
  vcl_vector<Inp> scan(4*width);
  for (int j=0; j<height; ++j) {
    if (!image.get_section(/* xxx */&scan[0], x0, y0+j, width, 1))
      return false;
    for (int i=0; i<width; ++i) {
      Inp r(scan[4*i+0]);
      Inp g(scan[4*i+1]);
      Inp b(scan[4*i+2]);
      // Weights convert from linear RGB to CIE luminance assuming a
      // modern monitor.  See Charles Pontyon's Colour FAQ
      // http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html
      static_cast<Out*>(buf)[i + width*j] = Out(0.2125*r+0.7154*g+0.072*b);
      // This are the old NTSC weights.
      //static_cast<Out*>(buf)[i + width*j] = Out(0.5+r*0.299+0.587*g+0.114*b);
    }
  }
  return true;
}


// Explicitly instantiate the ones we use

//template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vil_byte*,vil_byte*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_16*,vil_byte*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_32*,vil_byte*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, float*,vil_byte*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, double*,vil_byte*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, vil_byte*,vil_byte*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_16*,vil_byte*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, float*,vil_byte*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, double*,vil_byte*);
template bool convert_rgba_to_grey( const vil_image&, void*, int, int, int, int, vil_byte*,vil_byte*);

template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vil_byte*,vxl_uint_16*);
//template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_16*,vxl_uint_16*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_32*,vxl_uint_16*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, float*,vxl_uint_16*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, double*,vxl_uint_16*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, vil_byte*,vxl_uint_16*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_16*,vxl_uint_16*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, float*,vxl_uint_16*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, double*,vxl_uint_16*);

template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vil_byte*,int*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_16*,int*);
//template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_32*,int*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, float*,int*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, double*,int*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, vil_byte*,int*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_16*,int*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, float*,int*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, double*,int*);

template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vil_byte*,float*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_16*,float*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_32*,float*);
//template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, float*,float*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, double*,float*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, vil_byte*,float*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_16*,float*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, float*,float*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, double*,float*);

template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vil_byte*,double*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_16*,double*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_32*,double*);
template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, float*,double*);
//template bool convert_grey_to_grey( const vil_image&, void*, int, int, int, int, double*,double*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, vil_byte*,double*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, vxl_uint_16*,double*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, float*,double*);
template bool convert_rgb_to_grey( const vil_image&, void*, int, int, int, int, double*,double*);

template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, vil_byte*,unsigned char* );
template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, vxl_uint_16*,unsigned char* );
template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, vxl_uint_32*,unsigned char* );
template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, float*,unsigned char* );
template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, double*,unsigned char* );
//template bool convert_rgb_to_rgb( const vil_image&, void*, int, int, int, int, vil_byte*,unsigned char* );
template bool convert_rgb_to_rgb( const vil_image&, void*, int, int, int, int, vxl_uint_16*,unsigned char* );
template bool convert_rgb_to_rgb( const vil_image&, void*, int, int, int, int, float*,unsigned char* );
template bool convert_rgb_to_rgb( const vil_image&, void*, int, int, int, int, double*,unsigned char* );
template bool convert_rgba_to_rgb( const vil_image&, void*, int, int, int, int, vil_byte*,unsigned char* );

template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, vil_byte*,vxl_uint_16*);
template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, vxl_uint_16*,vxl_uint_16*);
template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, vxl_uint_32*,vxl_uint_16*);
template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, float*,vxl_uint_16*);
template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, double*,vxl_uint_16*);
template bool convert_rgb_to_rgb( const vil_image&, void*, int, int, int, int, vil_byte*,vxl_uint_16*);
//template bool convert_rgb_to_rgb( const vil_image&, void*, int, int, int, int, vxl_uint_16*,vxl_uint_16*);
template bool convert_rgb_to_rgb( const vil_image&, void*, int, int, int, int, float*,vxl_uint_16*);
template bool convert_rgb_to_rgb( const vil_image&, void*, int, int, int, int, double*,vxl_uint_16*);
template bool convert_rgba_to_rgb( const vil_image&, void*, int, int, int, int, vil_byte*,vxl_uint_16*);

template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, vil_byte*,float*);
template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, vxl_uint_16*,float*);
template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, vxl_uint_32*,float*);
template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, float*,float*);
template bool convert_grey_to_rgb( const vil_image&, void*, int, int, int, int, double*,float*);
template bool convert_rgb_to_rgb( const vil_image&, void*, int, int, int, int, vil_byte*,float*);
template bool convert_rgb_to_rgb( const vil_image&, void*, int, int, int, int, vxl_uint_16*,float*);
//template bool convert_rgb_to_rgb( const vil_image&, void*, int, int, int, int, float*,float*);
template bool convert_rgb_to_rgb( const vil_image&, void*, int, int, int, int, double*,float*);
template bool convert_rgba_to_rgb( const vil_image&, void*, int, int, int, int, vil_byte*,float*);


//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for byte.
bool vil_image_as_impl<vil_byte>::get_section(void *buf, int x0, int y0, int width, int height) const {
  typedef vil_byte Outtype;

  switch( vil_pixel_format(image) ) {
  case VIL_BYTE:
    return image.get_section( buf, x0, y0, width, height );
  case VIL_UINT16:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0);
  case VIL_UINT32:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (vxl_uint_32*)0,(Outtype*)0);
  case VIL_FLOAT:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0);
  case VIL_DOUBLE:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0);
  case VIL_RGB_BYTE:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  case VIL_RGB_UINT16:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0);
  case VIL_RGB_FLOAT:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0);
  case VIL_RGB_DOUBLE:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0);
  case VIL_RGBA_BYTE:
    return convert_rgba_to_grey( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  default:
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
}
/* START_MANCHESTER_BINARY_IO_CODE */

VCL_DEFINE_SPECIALIZATION
vcl_string vil_image_as_impl<vil_byte>::is_a() const
{
  static const vcl_string class_name_="vil_image_as_impl<vil_byte>";
  return class_name_;
}

VCL_DEFINE_SPECIALIZATION
bool vil_image_as_impl<vil_byte>::is_class(vcl_string const& s) const
{
  return s==vil_image_as_impl<vil_byte>::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */

// instantiate for byte.
template struct vil_image_as_impl<vil_byte>;

vil_image vil_image_as_byte(vil_image const &image) {
  return vil_image(new vil_image_as_impl<vil_byte>(image));
}

VCL_DEFINE_SPECIALIZATION
vil_image vil_image_as(vil_image const &image, vil_byte*) {
  return vil_image(new vil_image_as_impl<vil_byte>(image));
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for vxl_uint_16.
bool vil_image_as_impl<vxl_uint_16>::get_section(void *buf, int x0, int y0, int width, int height) const {
  typedef vxl_uint_16 Outtype;

  switch( vil_pixel_format(image) ) {
  case VIL_BYTE:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  case VIL_UINT16:
    return image.get_section( buf, x0, y0, width, height );
  case VIL_UINT32:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (vxl_uint_32*)0,(Outtype*)0);
  case VIL_FLOAT:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0);
  case VIL_DOUBLE:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0);
  case VIL_RGB_BYTE:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  case VIL_RGB_UINT16:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0);
  case VIL_RGB_FLOAT:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0);
  case VIL_RGB_DOUBLE:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0);
  default:
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
}

/* START_MANCHESTER_BINARY_IO_CODE */

VCL_DEFINE_SPECIALIZATION
vcl_string vil_image_as_impl<vxl_uint_16>::is_a() const
{
  static const vcl_string class_name_="vil_image_as_impl<vxl_uint_16>";
  return class_name_;
}

VCL_DEFINE_SPECIALIZATION
bool vil_image_as_impl<vxl_uint_16>::is_class(vcl_string const& s) const
{
  return s==vil_image_as_impl<vxl_uint_16>::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */

// instantiate for vxl_uint_16
template struct vil_image_as_impl<vxl_uint_16>;

vil_image vil_image_as_uint16(vil_image const &image) {
  return vil_image(new vil_image_as_impl<vxl_uint_16>(image));
}

VCL_DEFINE_SPECIALIZATION
vil_image vil_image_as(vil_image const &image, vxl_uint_16*) {
  return vil_image(new vil_image_as_impl<vxl_uint_16>(image));
}


//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for int.
bool vil_image_as_impl<int>::get_section(void *buf, int x0, int y0, int width, int height) const {
  typedef int Outtype;

  switch( vil_pixel_format(image) ) {
  case VIL_BYTE:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  case VIL_UINT16:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0);
  case VIL_UINT32:
    return image.get_section( buf, x0, y0, width, height );
  case VIL_FLOAT:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0);
  case VIL_DOUBLE:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0);
  case VIL_RGB_BYTE:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  case VIL_RGB_UINT16:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0);
  case VIL_RGB_FLOAT:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0);
  case VIL_RGB_DOUBLE:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0);
  default:
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
}

/* START_MANCHESTER_BINARY_IO_CODE */

VCL_DEFINE_SPECIALIZATION
vcl_string vil_image_as_impl<int>::is_a() const
{
  static const vcl_string class_name_="vil_image_as_impl<int>";
  return class_name_;
}

VCL_DEFINE_SPECIALIZATION
bool vil_image_as_impl<int>::is_class(vcl_string const& s) const
{
  return s==vil_image_as_impl<int>::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */

// instantiate for int.
template struct vil_image_as_impl<int>;

vil_image vil_image_as_int(vil_image const &image) {
  return vil_image(new vil_image_as_impl<int>(image));
}

VCL_DEFINE_SPECIALIZATION
vil_image vil_image_as(vil_image const &image, int*) {
  return vil_image(new vil_image_as_impl<int>(image));
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for float.
bool vil_image_as_impl<float>::get_section(void *buf, int x0, int y0, int width, int height) const {
  typedef float Outtype;

  switch( vil_pixel_format(image) ) {
  case VIL_BYTE:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  case VIL_UINT16:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0);
  case VIL_UINT32:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (vxl_uint_32*)0,(Outtype*)0);
  case VIL_FLOAT:
    return image.get_section( buf, x0, y0, width, height );
  case VIL_DOUBLE:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0);
  case VIL_RGB_BYTE:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  case VIL_RGB_UINT16:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0);
  case VIL_RGB_FLOAT:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0);
  case VIL_RGB_DOUBLE:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0);
  default:
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
}

/* START_MANCHESTER_BINARY_IO_CODE */

VCL_DEFINE_SPECIALIZATION
vcl_string vil_image_as_impl<float>::is_a() const
{
  static const vcl_string class_name_="vil_image_as_impl<float>";
  return class_name_;
}

VCL_DEFINE_SPECIALIZATION
bool vil_image_as_impl<float>::is_class(vcl_string const& s) const
{
  return s==vil_image_as_impl<float>::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */

// instantiate for float.
template struct vil_image_as_impl<float>;

vil_image vil_image_as_float(vil_image const &image) {
  return vil_image(new vil_image_as_impl<float>(image));
}

VCL_DEFINE_SPECIALIZATION
vil_image vil_image_as(vil_image const &image, float*) {
  return vil_image(new vil_image_as_impl<float>(image));
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for double.
bool vil_image_as_impl<double>::get_section(void *buf, int x0, int y0, int width, int height) const {
  typedef double Outtype;

  switch( vil_pixel_format(image) ) {
  case VIL_BYTE:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  case VIL_UINT16:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0);
  case VIL_UINT32:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (vxl_uint_32*)0,(Outtype*)0);
  case VIL_FLOAT:
    return convert_grey_to_grey( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0);
  case VIL_DOUBLE:
    return image.get_section( buf, x0, y0, width, height );
  case VIL_RGB_BYTE:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  case VIL_RGB_UINT16:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0);
  case VIL_RGB_FLOAT:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0);
  case VIL_RGB_DOUBLE:
    return convert_rgb_to_grey( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0);
  default:
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
}


/* START_MANCHESTER_BINARY_IO_CODE */

VCL_DEFINE_SPECIALIZATION
vcl_string vil_image_as_impl<double>::is_a() const
{
  static const vcl_string class_name_="vil_image_as_impl<double>";
  return class_name_;
}

VCL_DEFINE_SPECIALIZATION
bool vil_image_as_impl<double>::is_class(vcl_string const& s) const
{
  return s==vil_image_as_impl<double>::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */

// instantiate for double.
template struct vil_image_as_impl<double>;

vil_image vil_image_as_double(vil_image const &image) {
  return vil_image(new vil_image_as_impl<double>(image));
}

VCL_DEFINE_SPECIALIZATION
vil_image vil_image_as(vil_image const &image, double*) {
  return vil_image(new vil_image_as_impl<double>(image));
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for rgb.
bool vil_image_as_impl<vil_rgb<unsigned char> >::get_section(void *buf,
                                                             int x0, int y0,
                                                             int width, int height) const
{
  typedef unsigned char Outtype;

  switch( vil_pixel_format(image) ) {
  case VIL_BYTE:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0 );
  case VIL_UINT16:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0 );
  case VIL_UINT32:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (vxl_uint_32*)0,(Outtype*)0 );
  case VIL_FLOAT:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0 );
  case VIL_DOUBLE:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0 );
  case VIL_RGB_BYTE:
    return image.get_section( buf, x0, y0, width, height );
  case VIL_RGB_UINT16:
    return convert_rgb_to_rgb( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0 );
  case VIL_RGB_FLOAT:
    return convert_rgb_to_rgb( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0 );
  case VIL_RGB_DOUBLE:
    return convert_rgb_to_rgb( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0 );
  case VIL_RGBA_BYTE:
    return convert_rgba_to_rgb( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0 );
  default:
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
}
/* START_MANCHESTER_BINARY_IO_CODE */

VCL_DEFINE_SPECIALIZATION
vcl_string vil_image_as_impl<vil_rgb<unsigned char> >::is_a() const
{
  static const vcl_string class_name_="vil_image_as_impl<vil_rgb<unsigned char> >";
  return class_name_;
}

VCL_DEFINE_SPECIALIZATION
bool vil_image_as_impl<vil_rgb<unsigned char> >::is_class(vcl_string const& s) const
{
  return s==vil_image_as_impl<vil_rgb<unsigned char> >::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */

// instantiate for vil_rgb_byte .
template struct vil_image_as_impl<vil_rgb<unsigned char> >;

vil_image vil_image_as_rgb_byte(vil_image const &image) {
  return vil_image(new vil_image_as_impl<vil_rgb<unsigned char> >(image));
}

VCL_DEFINE_SPECIALIZATION
vil_image vil_image_as(vil_image const &image, vil_rgb<vil_byte>*) {
  return vil_image(new vil_image_as_impl<vil_rgb<vil_byte> >(image));
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for rgb float.
bool vil_image_as_impl<vil_rgb<float> >::get_section(void *buf,
                                                     int x0, int y0,
                                                     int width, int height) const
{
  typedef float Outtype;

  switch( vil_pixel_format(image) ) {
  case VIL_BYTE:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  case VIL_UINT16:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0);
  case VIL_UINT32:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (vxl_uint_32*)0,(Outtype*)0);
  case VIL_FLOAT:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0);
  case VIL_DOUBLE:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0);
  case VIL_RGB_BYTE:
    return convert_rgb_to_rgb( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  case VIL_RGB_UINT16:
    return convert_rgb_to_rgb( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0);
  case VIL_RGB_FLOAT:
    return image.get_section( buf, x0, y0, width, height );
  case VIL_RGB_DOUBLE:
    return convert_rgb_to_rgb( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0);
  case VIL_RGBA_BYTE:
    return convert_rgba_to_rgb( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  default:
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
}


/* START_MANCHESTER_BINARY_IO_CODE */

VCL_DEFINE_SPECIALIZATION
vcl_string vil_image_as_impl<vil_rgb<float> >::is_a() const
{
  static const vcl_string class_name_="vil_image_as_impl<vil_rgb<float> >";
  return class_name_;
}

VCL_DEFINE_SPECIALIZATION
bool vil_image_as_impl<vil_rgb<float> >::is_class(vcl_string const& s) const
{
  return s==vil_image_as_impl<vil_rgb<float> >::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */

// instantiate for vil_rgb_float .
template struct vil_image_as_impl<vil_rgb<float> >;

vil_image vil_image_as_rgb_float(vil_image const &image) {
  return vil_image(new vil_image_as_impl<vil_rgb<float> >(image));
}

VCL_DEFINE_SPECIALIZATION
vil_image vil_image_as(vil_image const &image, vil_rgb<float>*) {
  return vil_image(new vil_image_as_impl<vil_rgb<float> >(image));
}


//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION // specialize for rgb 16-bit.
bool vil_image_as_impl<vil_rgb<vxl_uint_16> >::get_section(void *buf,
                                                           int x0, int y0,
                                                           int width, int height) const
{
  typedef vxl_uint_16 Outtype;

  switch( vil_pixel_format(image) ) {
  case VIL_BYTE:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  case VIL_UINT16:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (vxl_uint_16*)0,(Outtype*)0);
  case VIL_UINT32:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (vxl_uint_32*)0,(Outtype*)0);
  case VIL_FLOAT:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0);
  case VIL_DOUBLE:
    return convert_grey_to_rgb( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0);
  case VIL_RGB_BYTE:
    return convert_rgb_to_rgb( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  case VIL_RGB_UINT16:
    return image.get_section( buf, x0, y0, width, height );
  case VIL_RGB_FLOAT:
    return convert_rgb_to_rgb( image, buf, x0, y0, width, height, (float*)0,(Outtype*)0);
  case VIL_RGB_DOUBLE:
    return convert_rgb_to_rgb( image, buf, x0, y0, width, height, (double*)0,(Outtype*)0);
  case VIL_RGBA_BYTE:
    return convert_rgba_to_rgb( image, buf, x0, y0, width, height, (vil_byte*)0,(Outtype*)0);
  default:
    vcl_cerr << __FILE__ ": get_section() not implemented for " << image << vcl_endl;
    assert(false/* implement for your image type as needed */);
    return false;
  }
}


VCL_DEFINE_SPECIALIZATION
vcl_string vil_image_as_impl<vil_rgb<vxl_uint_16> >::is_a() const
{
  static const vcl_string class_name_="vil_image_as_impl<vil_rgb<vxl_uint_16> >";
  return class_name_;
}

VCL_DEFINE_SPECIALIZATION
bool vil_image_as_impl<vil_rgb<vxl_uint_16> >::is_class(vcl_string const& s) const
{
  return s==vil_image_as_impl<vil_rgb<vxl_uint_16> >::is_a() || vil_image_impl::is_class(s);
}


// instantiate for vil_rgb_uint16 .
template struct vil_image_as_impl<vil_rgb<vxl_uint_16> >;

vil_image vil_image_as_rgb_uint16(vil_image const &image) {
  return vil_image(new vil_image_as_impl<vil_rgb<vxl_uint_16> >(image));
}

VCL_DEFINE_SPECIALIZATION
vil_image vil_image_as(vil_image const &image, vil_rgb<vxl_uint_16>* ) {
  return vil_image(new vil_image_as_impl<vil_rgb<vxl_uint_16> >(image));
}
