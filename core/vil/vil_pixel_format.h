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

  VIL2_PIXEL_FORMAT_RGB_UNSIGNED_INT,
  VIL2_PIXEL_FORMAT_RGB_SIGNED_INT,
  VIL2_PIXEL_FORMAT_RGB_UNSIGNED_SHORT,
  VIL2_PIXEL_FORMAT_RGB_SIGNED_SHORT,
  VIL2_PIXEL_FORMAT_RGB_BYTE,
  VIL2_PIXEL_FORMAT_RGB_SIGNED_CHAR,
  VIL2_PIXEL_FORMAT_RGB_FLOAT,
  VIL2_PIXEL_FORMAT_RGB_DOUBLE
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

vil2_pixel_format_macro(vil_rgb<unsigned int>,   VIL2_PIXEL_FORMAT_RGB_UNSIGNED_INT)
vil2_pixel_format_macro(vil_rgb<signed int>,     VIL2_PIXEL_FORMAT_RGB_SIGNED_INT)
vil2_pixel_format_macro(vil_rgb<unsigned short>, VIL2_PIXEL_FORMAT_RGB_UNSIGNED_SHORT)
vil2_pixel_format_macro(vil_rgb<signed short>,   VIL2_PIXEL_FORMAT_RGB_SIGNED_SHORT)
vil2_pixel_format_macro(vil_rgb<vil_byte>,       VIL2_PIXEL_FORMAT_RGB_BYTE)
vil2_pixel_format_macro(vil_rgb<signed char>,    VIL2_PIXEL_FORMAT_RGB_SIGNED_CHAR)
vil2_pixel_format_macro(vil_rgb<float>,          VIL2_PIXEL_FORMAT_RGB_DOUBLE)
vil2_pixel_format_macro(vil_rgb<double>,         VIL2_PIXEL_FORMAT_RGB_FLOAT)


inline
vcl_ostream & operator << (vcl_ostream &os, vil2_pixel_format f)
{
  switch(f) {
    case VIL2_PIXEL_FORMAT_UNKNOWN:        os << "VIL2_PIXEL_FORMAT=UNKNOWN";
    case VIL2_PIXEL_FORMAT_UNSIGNED_INT:   os << "VIL2_PIXEL_FORMAT=unsigned int";
    case VIL2_PIXEL_FORMAT_SIGNED_INT:     os << "VIL2_PIXEL_FORMAT=signed int";
    case VIL2_PIXEL_FORMAT_UNSIGNED_SHORT: os << "VIL2_PIXEL_FORMAT=unsigned short";
    case VIL2_PIXEL_FORMAT_SIGNED_SHORT:   os << "VIL2_PIXEL_FORMAT=signed short";
    case VIL2_PIXEL_FORMAT_FLOAT:          os << "VIL2_PIXEL_FORMAT=float";
    case VIL2_PIXEL_FORMAT_DOUBLE:         os << "VIL2_PIXEL_FORMAT=double";
    case VIL2_PIXEL_FORMAT_BYTE:           os << "VIL2_PIXEL_FORMAT=vil_byte";
    case VIL2_PIXEL_FORMAT_SIGNED_CHAR:    os << "VIL2_PIXEL_FORMAT=signed char";

    case VIL2_PIXEL_FORMAT_RGB_UNSIGNED_INT:   os << "VIL2_PIXEL_FORMAT=vil_rgb<unsigned int>";
    case VIL2_PIXEL_FORMAT_RGB_SIGNED_INT:     os << "VIL2_PIXEL_FORMAT=vil_rgb<signed int>";
    case VIL2_PIXEL_FORMAT_RGB_UNSIGNED_SHORT: os << "VIL2_PIXEL_FORMAT=vil_rgb<unsigned short>";
    case VIL2_PIXEL_FORMAT_RGB_SIGNED_SHORT:   os << "VIL2_PIXEL_FORMAT=vil_rgb<signed short>";
    case VIL2_PIXEL_FORMAT_RGB_FLOAT:          os << "VIL2_PIXEL_FORMAT=vil_rgb<float>";
    case VIL2_PIXEL_FORMAT_RGB_DOUBLE:         os << "VIL2_PIXEL_FORMAT=vil_rgb<double>";
    case VIL2_PIXEL_FORMAT_RGB_BYTE:           os << "VIL2_PIXEL_FORMAT=vil_rgb<vil_byte>";
    case VIL2_PIXEL_FORMAT_RGB_SIGNED_CHAR:    os << "VIL2_PIXEL_FORMAT=vil_rgb<signed char>";
    default:  os << "VIL2_PIXEL_FORMAT_INVALID";
  }
  return os;
}

#endif // vil2_pixel_format
