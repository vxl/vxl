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
  VIL2_PIXEL_FORMAT_UNKNOWN,

  VIL2_PIXEL_FORMAT_UINT_32,
  VIL2_PIXEL_FORMAT_INT_32,
  VIL2_PIXEL_FORMAT_UINT_16,
  VIL2_PIXEL_FORMAT_INT_16,
  VIL2_PIXEL_FORMAT_BYTE,
  VIL2_PIXEL_FORMAT_INT_8,
  VIL2_PIXEL_FORMAT_FLOAT,
  VIL2_PIXEL_FORMAT_DOUBLE,
  VIL2_PIXEL_FORMAT_BOOL,

  VIL2_PIXEL_FORMAT_RGB_UINT_32,
  VIL2_PIXEL_FORMAT_RGB_INT_32,
  VIL2_PIXEL_FORMAT_RGB_UINT_16,
  VIL2_PIXEL_FORMAT_RGB_INT_16,
  VIL2_PIXEL_FORMAT_RGB_BYTE,
  VIL2_PIXEL_FORMAT_RGB_INT_8,
  VIL2_PIXEL_FORMAT_RGB_FLOAT,
  VIL2_PIXEL_FORMAT_RGB_DOUBLE,

  VIL2_PIXEL_FORMAT_RGBA_UINT_32,
  VIL2_PIXEL_FORMAT_RGBA_INT_32,
  VIL2_PIXEL_FORMAT_RGBA_UINT_16,
  VIL2_PIXEL_FORMAT_RGBA_INT_16,
  VIL2_PIXEL_FORMAT_RGBA_BYTE,
  VIL2_PIXEL_FORMAT_RGBA_INT_8,
  VIL2_PIXEL_FORMAT_RGBA_FLOAT,
  VIL2_PIXEL_FORMAT_RGBA_DOUBLE,
// Add values here, and be careful to keep values in vil2_ixel_format.cxx in sync

  VIL2_PIXEL_FORMAT_ENUM_END
}; 



//: Get the vil2_pixel_format value for a given type.
#define vil2_pixel_format_macro(T,V)\
inline enum vil2_pixel_format vil2_pixel_format_of(T ) { return V; }

vil2_pixel_format_macro(vxl_uint_32, VIL2_PIXEL_FORMAT_UINT_32)
vil2_pixel_format_macro(vxl_int_32,  VIL2_PIXEL_FORMAT_INT_32)
vil2_pixel_format_macro(vxl_uint_16, VIL2_PIXEL_FORMAT_UINT_16)
vil2_pixel_format_macro(vxl_int_16,  VIL2_PIXEL_FORMAT_INT_16)
vil2_pixel_format_macro(vil2_byte,   VIL2_PIXEL_FORMAT_BYTE)
vil2_pixel_format_macro(vxl_int_8,  VIL2_PIXEL_FORMAT_INT_8)
vil2_pixel_format_macro(float,       VIL2_PIXEL_FORMAT_DOUBLE)
vil2_pixel_format_macro(double,      VIL2_PIXEL_FORMAT_FLOAT)
vil2_pixel_format_macro(bool,        VIL2_PIXEL_FORMAT_BOOL)

vil2_pixel_format_macro(vil_rgb<vxl_uint_32>, VIL2_PIXEL_FORMAT_RGB_UINT_32)
vil2_pixel_format_macro(vil_rgb<vxl_int_32>,  VIL2_PIXEL_FORMAT_RGB_INT_32)
vil2_pixel_format_macro(vil_rgb<vxl_uint_16>, VIL2_PIXEL_FORMAT_RGB_UINT_16)
vil2_pixel_format_macro(vil_rgb<vxl_int_16>,  VIL2_PIXEL_FORMAT_RGB_INT_16)
vil2_pixel_format_macro(vil_rgb<vil2_byte>,   VIL2_PIXEL_FORMAT_RGB_BYTE)
vil2_pixel_format_macro(vil_rgb<vxl_int_8>,  VIL2_PIXEL_FORMAT_RGB_INT_8)
vil2_pixel_format_macro(vil_rgb<float>,       VIL2_PIXEL_FORMAT_RGB_DOUBLE)
vil2_pixel_format_macro(vil_rgb<double>,      VIL2_PIXEL_FORMAT_RGB_FLOAT)

vil2_pixel_format_macro(vil_rgba<vxl_uint_32>, VIL2_PIXEL_FORMAT_RGBA_UINT_32)
vil2_pixel_format_macro(vil_rgba<vxl_int_32>,  VIL2_PIXEL_FORMAT_RGBA_INT_32)
vil2_pixel_format_macro(vil_rgba<vxl_uint_16>, VIL2_PIXEL_FORMAT_RGBA_UINT_16)
vil2_pixel_format_macro(vil_rgba<vxl_int_16>,  VIL2_PIXEL_FORMAT_RGBA_INT_16)
vil2_pixel_format_macro(vil_rgba<vil2_byte>,   VIL2_PIXEL_FORMAT_RGBA_BYTE)
vil2_pixel_format_macro(vil_rgba<vxl_int_8>,  VIL2_PIXEL_FORMAT_RGBA_INT_8)
vil2_pixel_format_macro(vil_rgba<float>,       VIL2_PIXEL_FORMAT_RGBA_DOUBLE)
vil2_pixel_format_macro(vil_rgba<double>,      VIL2_PIXEL_FORMAT_RGBA_FLOAT)

#undef vil2_pixel_format_macro

//: Return the number of bytes used by each component of pixel format f
unsigned vil2_pixel_format_sizeof_components(enum vil2_pixel_format f);
//: Return the number of components in pixel format f
unsigned vil2_pixel_format_num_components(enum vil2_pixel_format f);

//: output a pretty string representing the pixel format.
vcl_ostream & operator << (vcl_ostream &os, vil2_pixel_format f);

#endif // vil2_pixel_format
