// This is mul/vil2/vil2_pixel_format.h
#ifndef vil2_pixel_format_h_
#define vil2_pixel_format_h_
//:
// \file
// \author Ian Scott.

#include <vil/vil_rgb.h>
#include <vil/vil_byte.h>

//: Describes the type of the concrete data.
enum vil2_pixel_format {
  VIL2_PIXEL_FORMAT_UNKNOWN,

  VIL2_PIXEL_FORMAT_UNSIGNED_INT,
  VIL2_PIXEL_FORMAT_SIGNED_INT,
  VIL2_PIXEL_FORMAT_UNSIGNED_SHORT,
  VIL2_PIXEL_FORMAT_SIGNED_SHORT,
  VIL2_PIXEL_FORMAT_BYTE,
  VIL2_PIXEL_FORMAT_SIGNED_CHAR,
  VIL2_PIXEL_FORMAT_FLOAT,
  VIL2_PIXEL_FORMAT_DOUBLE,
  VIL2_PIXEL_FORMAT_BOOL,

  VIL2_PIXEL_FORMAT_RGB_UNSIGNED_INT,
  VIL2_PIXEL_FORMAT_RGB_SIGNED_INT,
  VIL2_PIXEL_FORMAT_RGB_UNSIGNED_SHORT,
  VIL2_PIXEL_FORMAT_RGB_SIGNED_SHORT,
  VIL2_PIXEL_FORMAT_RGB_BYTE,
  VIL2_PIXEL_FORMAT_RGB_SIGNED_CHAR,
  VIL2_PIXEL_FORMAT_RGB_FLOAT,
  VIL2_PIXEL_FORMAT_RGB_DOUBLE,
// Add values here, and be careful to keep values in vil2_ixel_format.cxx in sync

  VIL2_PIXEL_FORMAT_ENUM_END
}; 



//: Get the vil2_pixel_format value for a given type.
#define vil2_pixel_format_macro(T,V)\
inline enum vil2_pixel_format vil2_pixel_format_of(T ) { return V; }

vil2_pixel_format_macro(unsigned int,   VIL2_PIXEL_FORMAT_UNSIGNED_INT)
vil2_pixel_format_macro(signed int,     VIL2_PIXEL_FORMAT_SIGNED_INT)
vil2_pixel_format_macro(unsigned short, VIL2_PIXEL_FORMAT_UNSIGNED_SHORT)
vil2_pixel_format_macro(signed short,   VIL2_PIXEL_FORMAT_SIGNED_SHORT)
vil2_pixel_format_macro(vil_byte,       VIL2_PIXEL_FORMAT_BYTE)
vil2_pixel_format_macro(signed char,    VIL2_PIXEL_FORMAT_SIGNED_CHAR)
vil2_pixel_format_macro(float,          VIL2_PIXEL_FORMAT_DOUBLE)
vil2_pixel_format_macro(double,         VIL2_PIXEL_FORMAT_FLOAT)
vil2_pixel_format_macro(bool,           VIL2_PIXEL_FORMAT_BOOL)

vil2_pixel_format_macro(vil_rgb<unsigned int>,   VIL2_PIXEL_FORMAT_RGB_UNSIGNED_INT)
vil2_pixel_format_macro(vil_rgb<signed int>,     VIL2_PIXEL_FORMAT_RGB_SIGNED_INT)
vil2_pixel_format_macro(vil_rgb<unsigned short>, VIL2_PIXEL_FORMAT_RGB_UNSIGNED_SHORT)
vil2_pixel_format_macro(vil_rgb<signed short>,   VIL2_PIXEL_FORMAT_RGB_SIGNED_SHORT)
vil2_pixel_format_macro(vil_rgb<vil_byte>,       VIL2_PIXEL_FORMAT_RGB_BYTE)
vil2_pixel_format_macro(vil_rgb<signed char>,    VIL2_PIXEL_FORMAT_RGB_SIGNED_CHAR)
vil2_pixel_format_macro(vil_rgb<float>,          VIL2_PIXEL_FORMAT_RGB_DOUBLE)
vil2_pixel_format_macro(vil_rgb<double>,         VIL2_PIXEL_FORMAT_RGB_FLOAT)

#undef vil2_pixel_format_macro

//: Return the number of bytes used by each component of pixel format f
unsigned vil2_pixel_format_sizeof_components(enum vil2_pixel_format f);
//: Return the number of components in pixel format f
unsigned vil2_pixel_format_num_components(enum vil2_pixel_format f);

//: output a pretty string representing the pixel format.
vcl_ostream & operator << (vcl_ostream &os, vil2_pixel_format f);

#endif // vil2_pixel_format
