// This is mul/vil2/vil2_pixel_format.h
#ifndef vil2_pixel_format_h_
#define vil2_pixel_format_h_
//:
// \file
// \author Ian Scott.

#include <vxl_config.h>
#include <vil/vil_rgb.h>
#include <vil/vil_rgba.h>
#include <vil2/vil2_byte.h>

//: Describes the type of the concrete data.
enum vil2_pixel_format {
  VIL2_PIXEL_FORMAT_UNKNOWN = 0,

//  VIL2_PIXEL_FORMAT_UINT_64 = 1,
//  VIL2_PIXEL_FORMAT_INT_64 = 2,
  VIL2_PIXEL_FORMAT_UINT_32 = 3,
  VIL2_PIXEL_FORMAT_INT_32 = 4,
  VIL2_PIXEL_FORMAT_UINT_16 = 5,
  VIL2_PIXEL_FORMAT_INT_16 = 6,
  VIL2_PIXEL_FORMAT_BYTE = 7,
  VIL2_PIXEL_FORMAT_INT_8 = 8,
  VIL2_PIXEL_FORMAT_FLOAT = 9,
  VIL2_PIXEL_FORMAT_DOUBLE = 10,
//  VIL2_PIXEL_FORMAT_LONG_DOUBLE = 11,
  VIL2_PIXEL_FORMAT_BOOL = 12,

//  VIL2_PIXEL_FORMAT_RGB_UINT_64 = 13,
//  VIL2_PIXEL_FORMAT_RGB_INT_64 = 14,
  VIL2_PIXEL_FORMAT_RGB_UINT_32 = 15,
  VIL2_PIXEL_FORMAT_RGB_INT_32 = 16,
  VIL2_PIXEL_FORMAT_RGB_UINT_16 = 17,
  VIL2_PIXEL_FORMAT_RGB_INT_16 = 18,
  VIL2_PIXEL_FORMAT_RGB_BYTE = 19,
  VIL2_PIXEL_FORMAT_RGB_INT_8 = 20,
  VIL2_PIXEL_FORMAT_RGB_FLOAT = 21,
  VIL2_PIXEL_FORMAT_RGB_DOUBLE = 22,
//  VIL2_PIXEL_FORMAT_RGB_LONG_DOUBLE = 23,

//  VIL2_PIXEL_FORMAT_RGBA_UINT_64 = 24,
//  VIL2_PIXEL_FORMAT_RGBA_INT_64 = 25,
  VIL2_PIXEL_FORMAT_RGBA_UINT_32 = 26,
  VIL2_PIXEL_FORMAT_RGBA_INT_32 = 27,
  VIL2_PIXEL_FORMAT_RGBA_UINT_16 = 28,
  VIL2_PIXEL_FORMAT_RGBA_INT_16 = 29,
  VIL2_PIXEL_FORMAT_RGBA_BYTE = 30,
  VIL2_PIXEL_FORMAT_RGBA_INT_8 = 31,
  VIL2_PIXEL_FORMAT_RGBA_FLOAT = 32,
  VIL2_PIXEL_FORMAT_RGBA_DOUBLE = 33,
//  VIL2_PIXEL_FORMAT_RGBA_LONG_DOUBLE = 34,
// Add values here and be careful to keep values in vil2_pixel_format.cxx in sync
// Don't forget to increase the end value.



  VIL2_PIXEL_FORMAT_ENUM_END = 35
}; 



template <class T>
inline vil2_pixel_format vil2_pixel_format_of(T dummy) { return VIL2_PIXEL_FORMAT_UNKNOWN;}

//: Get the vil2_pixel_format value for a given type.
#define vil2_pixel_format_macro(T,V)\
VCL_DEFINE_SPECIALIZATION inline vil2_pixel_format vil2_pixel_format_of(T dummy) { return V; }

vil2_pixel_format_macro(vxl_uint_32, VIL2_PIXEL_FORMAT_UINT_32)
vil2_pixel_format_macro(vxl_int_32,  VIL2_PIXEL_FORMAT_INT_32)
vil2_pixel_format_macro(vxl_uint_16, VIL2_PIXEL_FORMAT_UINT_16)
vil2_pixel_format_macro(vxl_int_16,  VIL2_PIXEL_FORMAT_INT_16)
vil2_pixel_format_macro(vil2_byte,   VIL2_PIXEL_FORMAT_BYTE)
vil2_pixel_format_macro(vxl_int_8,   VIL2_PIXEL_FORMAT_INT_8)
vil2_pixel_format_macro(float,       VIL2_PIXEL_FORMAT_FLOAT)
vil2_pixel_format_macro(double,      VIL2_PIXEL_FORMAT_DOUBLE)
vil2_pixel_format_macro(bool,        VIL2_PIXEL_FORMAT_BOOL)

vil2_pixel_format_macro(vil_rgb<vxl_uint_32>, VIL2_PIXEL_FORMAT_RGB_UINT_32)
vil2_pixel_format_macro(vil_rgb<vxl_int_32>,  VIL2_PIXEL_FORMAT_RGB_INT_32)
vil2_pixel_format_macro(vil_rgb<vxl_uint_16>, VIL2_PIXEL_FORMAT_RGB_UINT_16)
vil2_pixel_format_macro(vil_rgb<vxl_int_16>,  VIL2_PIXEL_FORMAT_RGB_INT_16)
vil2_pixel_format_macro(vil_rgb<vil2_byte>,   VIL2_PIXEL_FORMAT_RGB_BYTE)
vil2_pixel_format_macro(vil_rgb<vxl_int_8>,   VIL2_PIXEL_FORMAT_RGB_INT_8)
vil2_pixel_format_macro(vil_rgb<float>,       VIL2_PIXEL_FORMAT_RGB_FLOAT)
vil2_pixel_format_macro(vil_rgb<double>,      VIL2_PIXEL_FORMAT_RGB_DOUBLE)

vil2_pixel_format_macro(vil_rgba<vxl_uint_32>, VIL2_PIXEL_FORMAT_RGBA_UINT_32)
vil2_pixel_format_macro(vil_rgba<vxl_int_32>,  VIL2_PIXEL_FORMAT_RGBA_INT_32)
vil2_pixel_format_macro(vil_rgba<vxl_uint_16>, VIL2_PIXEL_FORMAT_RGBA_UINT_16)
vil2_pixel_format_macro(vil_rgba<vxl_int_16>,  VIL2_PIXEL_FORMAT_RGBA_INT_16)
vil2_pixel_format_macro(vil_rgba<vil2_byte>,   VIL2_PIXEL_FORMAT_RGBA_BYTE)
vil2_pixel_format_macro(vil_rgba<vxl_int_8>,   VIL2_PIXEL_FORMAT_RGBA_INT_8)
vil2_pixel_format_macro(vil_rgba<float>,       VIL2_PIXEL_FORMAT_RGBA_FLOAT)
vil2_pixel_format_macro(vil_rgba<double>,      VIL2_PIXEL_FORMAT_RGBA_DOUBLE)

#undef vil2_pixel_format_macro

//: Return the number of bytes used by each component of pixel format f
unsigned vil2_pixel_format_sizeof_components(enum vil2_pixel_format f);

//: Return the number of components in pixel format f
unsigned vil2_pixel_format_num_components(enum vil2_pixel_format f);

//: Return the number of components in pixel format f
vil2_pixel_format vil2_pixel_format_component_format(enum vil2_pixel_format f);

//: Output a pretty string representing the pixel format.
vcl_ostream & operator << (vcl_ostream &os, vil2_pixel_format f);

#endif // vil2_pixel_format
