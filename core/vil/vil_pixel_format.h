// This is core/vil/vil_pixel_format.h
#ifndef vil_pixel_format_h_
#define vil_pixel_format_h_
//:
// \file
// \author Ian Scott.
// Note that a vcl_complex<float> is thought of as a scalar
// pixel type for vil's purposes.
//
// \verbatim
//  Modifications
//   23 Oct.2003 - Peter Vanroose - Added support for 64-bit int pixels
// \endvarbatim

#include <vil/vil_rgb.h>
#include <vil/vil_rgba.h>
#include <vxl_config.h> // for vxl_uint_32 etc.
#include <vcl_iosfwd.h>
#include <vcl_complex.h>

//: Describes the type of the concrete data.
enum vil_pixel_format
{
  VIL_PIXEL_FORMAT_UNKNOWN = 0,

#if VXL_HAS_INT_64
  VIL_PIXEL_FORMAT_UINT_64 = 1,
  VIL_PIXEL_FORMAT_INT_64 = 2,
#endif
  VIL_PIXEL_FORMAT_UINT_32 = 3,
  VIL_PIXEL_FORMAT_INT_32 = 4,
  VIL_PIXEL_FORMAT_UINT_16 = 5,
  VIL_PIXEL_FORMAT_INT_16 = 6,
  VIL_PIXEL_FORMAT_BYTE = 7,
  VIL_PIXEL_FORMAT_SBYTE = 8,
  VIL_PIXEL_FORMAT_FLOAT = 9,
  VIL_PIXEL_FORMAT_DOUBLE = 10,
//  VIL_PIXEL_FORMAT_LONG_DOUBLE = 11,
  VIL_PIXEL_FORMAT_BOOL = 12,

#if VXL_HAS_INT_64
  VIL_PIXEL_FORMAT_RGB_UINT_64 = 13,
  VIL_PIXEL_FORMAT_RGB_INT_64 = 14,
#endif
  VIL_PIXEL_FORMAT_RGB_UINT_32 = 15,
  VIL_PIXEL_FORMAT_RGB_INT_32 = 16,
  VIL_PIXEL_FORMAT_RGB_UINT_16 = 17,
  VIL_PIXEL_FORMAT_RGB_INT_16 = 18,
  VIL_PIXEL_FORMAT_RGB_BYTE = 19,
  VIL_PIXEL_FORMAT_RGB_SBYTE = 20,
  VIL_PIXEL_FORMAT_RGB_FLOAT = 21,
  VIL_PIXEL_FORMAT_RGB_DOUBLE = 22,
//  VIL_PIXEL_FORMAT_RGB_LONG_DOUBLE = 23,

#if VXL_HAS_INT_64
  VIL_PIXEL_FORMAT_RGBA_UINT_64 = 24,
  VIL_PIXEL_FORMAT_RGBA_INT_64 = 25,
#endif
  VIL_PIXEL_FORMAT_RGBA_UINT_32 = 26,
  VIL_PIXEL_FORMAT_RGBA_INT_32 = 27,
  VIL_PIXEL_FORMAT_RGBA_UINT_16 = 28,
  VIL_PIXEL_FORMAT_RGBA_INT_16 = 29,
  VIL_PIXEL_FORMAT_RGBA_BYTE = 30,
  VIL_PIXEL_FORMAT_RGBA_SBYTE = 31,
  VIL_PIXEL_FORMAT_RGBA_FLOAT = 32,
  VIL_PIXEL_FORMAT_RGBA_DOUBLE = 33,
//  VIL_PIXEL_FORMAT_RGBA_LONG_DOUBLE = 34,

//: vcl_complex<float> is a scalar for vil's purposes.
  VIL_PIXEL_FORMAT_COMPLEX_FLOAT = 35,
//: vcl_complex<double> is a scalar for vil's purposes.
  VIL_PIXEL_FORMAT_COMPLEX_DOUBLE = 36,

// Add values here and be careful to keep values in vil_pixel_format.cxx in sync
// Don't forget to increase the end value. Also add to vil_convert_cast in vil_convert.h

  VIL_PIXEL_FORMAT_ENUM_END = 37
};


//: The pixel format enumeration corresponding to the C++ type.
//
template <class T>
inline vil_pixel_format vil_pixel_format_of(T){return VIL_PIXEL_FORMAT_UNKNOWN;}


//: The C++ type corresponding to an invalid pixel format
//
// See vil_pixel_format_type_of.
//
typedef void* vil_pixel_format_invalid_type;

//: The C++ type corresponding to a pixel format enumeration.
// Use like
// \code
//    typedef vil_pixel_format_type_of<VIL_PIXEL_FORMAT_BYTE>::type byte_type;
// \endcode
// This is specialized for each pixel type enumeration for which a C++
// type exists.
//
// If the resulting type is vil_pixel_format_invalid_type, then the
// pixel format enumeration is not valid.
//
template <vil_pixel_format pix_type>
struct vil_pixel_format_type_of
{
  typedef vil_pixel_format_invalid_type type;
  typedef vil_pixel_format_invalid_type component_type;
};


//: Get the vil_pixel_format value for a given type.
#define vil_pixel_format_macro(T,C,V)\
VCL_DEFINE_SPECIALIZATION inline vil_pixel_format vil_pixel_format_of(T /*dummy*/) { return V; }\
VCL_DEFINE_SPECIALIZATION struct vil_pixel_format_type_of<V> { typedef T type; typedef C component_type; }

#if VXL_HAS_INT_64
vil_pixel_format_macro(vxl_uint_64, vxl_uint_64, VIL_PIXEL_FORMAT_UINT_64);
vil_pixel_format_macro(vxl_int_64,  vxl_int_64,  VIL_PIXEL_FORMAT_INT_64);
#endif
vil_pixel_format_macro(vxl_uint_32, vxl_uint_32, VIL_PIXEL_FORMAT_UINT_32);
vil_pixel_format_macro(vxl_int_32,  vxl_int_32,  VIL_PIXEL_FORMAT_INT_32);
vil_pixel_format_macro(vxl_uint_16, vxl_uint_16, VIL_PIXEL_FORMAT_UINT_16);
vil_pixel_format_macro(vxl_int_16,  vxl_int_16,  VIL_PIXEL_FORMAT_INT_16);
vil_pixel_format_macro(vxl_byte,    vxl_byte,    VIL_PIXEL_FORMAT_BYTE);
vil_pixel_format_macro(vxl_sbyte,   vxl_sbyte,   VIL_PIXEL_FORMAT_SBYTE);
vil_pixel_format_macro(float,       float,       VIL_PIXEL_FORMAT_FLOAT);
vil_pixel_format_macro(double,      double,      VIL_PIXEL_FORMAT_DOUBLE);
vil_pixel_format_macro(bool,        bool,        VIL_PIXEL_FORMAT_BOOL);

#if VXL_HAS_INT_64
vil_pixel_format_macro(vil_rgb<vxl_uint_64>, vxl_uint_64, VIL_PIXEL_FORMAT_RGB_UINT_64);
vil_pixel_format_macro(vil_rgb<vxl_int_64>,  vxl_int_64,  VIL_PIXEL_FORMAT_RGB_INT_64);
#endif
vil_pixel_format_macro(vil_rgb<vxl_uint_32>, vxl_uint_32, VIL_PIXEL_FORMAT_RGB_UINT_32);
vil_pixel_format_macro(vil_rgb<vxl_int_32>,  vxl_int_32,  VIL_PIXEL_FORMAT_RGB_INT_32);
vil_pixel_format_macro(vil_rgb<vxl_uint_16>, vxl_uint_16, VIL_PIXEL_FORMAT_RGB_UINT_16);
vil_pixel_format_macro(vil_rgb<vxl_int_16>,  vxl_int_16,  VIL_PIXEL_FORMAT_RGB_INT_16);
vil_pixel_format_macro(vil_rgb<vxl_byte>,    vxl_byte,    VIL_PIXEL_FORMAT_RGB_BYTE);
vil_pixel_format_macro(vil_rgb<vxl_sbyte>,   vxl_sbyte,   VIL_PIXEL_FORMAT_RGB_SBYTE);
vil_pixel_format_macro(vil_rgb<float>,       float,       VIL_PIXEL_FORMAT_RGB_FLOAT);
vil_pixel_format_macro(vil_rgb<double>,      double,      VIL_PIXEL_FORMAT_RGB_DOUBLE);

#if VXL_HAS_INT_64
vil_pixel_format_macro(vil_rgba<vxl_uint_64>, vxl_uint_64, VIL_PIXEL_FORMAT_RGBA_UINT_64);
vil_pixel_format_macro(vil_rgba<vxl_int_64>,  vxl_int_64,  VIL_PIXEL_FORMAT_RGBA_INT_64);
#endif
vil_pixel_format_macro(vil_rgba<vxl_uint_32>, vxl_uint_32, VIL_PIXEL_FORMAT_RGBA_UINT_32);
vil_pixel_format_macro(vil_rgba<vxl_int_32>,  vxl_int_32,  VIL_PIXEL_FORMAT_RGBA_INT_32);
vil_pixel_format_macro(vil_rgba<vxl_uint_16>, vxl_uint_16, VIL_PIXEL_FORMAT_RGBA_UINT_16);
vil_pixel_format_macro(vil_rgba<vxl_int_16>,  vxl_int_16,  VIL_PIXEL_FORMAT_RGBA_INT_16);
vil_pixel_format_macro(vil_rgba<vxl_byte>,    vxl_byte,    VIL_PIXEL_FORMAT_RGBA_BYTE);
vil_pixel_format_macro(vil_rgba<vxl_sbyte>,   vxl_sbyte,   VIL_PIXEL_FORMAT_RGBA_SBYTE);
vil_pixel_format_macro(vil_rgba<float>,       float,       VIL_PIXEL_FORMAT_RGBA_FLOAT);
vil_pixel_format_macro(vil_rgba<double>,      double,      VIL_PIXEL_FORMAT_RGBA_DOUBLE);

vil_pixel_format_macro(vcl_complex<float>, vcl_complex<float>, VIL_PIXEL_FORMAT_COMPLEX_FLOAT);
vil_pixel_format_macro(vcl_complex<double>,vcl_complex<double>,VIL_PIXEL_FORMAT_COMPLEX_DOUBLE);

#undef vil_pixel_format_macro

//: Return the number of bytes used by each component of pixel format f
unsigned vil_pixel_format_sizeof_components(enum vil_pixel_format f);

//: Return the number of components in pixel format f
unsigned vil_pixel_format_num_components(enum vil_pixel_format f);

//: Return the format of each component of pixel format f
vil_pixel_format vil_pixel_format_component_format(enum vil_pixel_format f);

//: Output a pretty string representing the pixel format.
vcl_ostream & operator << (vcl_ostream &os, vil_pixel_format f);

#endif // vil_pixel_format_h_
