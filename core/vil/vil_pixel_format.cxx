// This is mul/vil2/vil2_pixel_format.cxx

//:
// \file
// \author Ian Scott.

#include "vil2_pixel_format.h"


static unsigned component_size[]={
  0,                //  VIL2_PIXEL_FORMAT_UNSIGNED_INT

  sizeof(int),      //  VIL2_PIXEL_FORMAT_UNSIGNED_INT
  sizeof(int),      //  VIL2_PIXEL_FORMAT_SIGNED_INT

  sizeof(short),    //  VIL2_PIXEL_FORMAT_UNSIGNED_SHORT
  sizeof(short),    //  VIL2_PIXEL_FORMAT_SIGNED_SHORT
  sizeof(char),     //  VIL2_PIXEL_FORMAT_BYTE
  sizeof(char),     //  VIL2_PIXEL_FORMAT_SIGNED_CHAR
  sizeof(float),    //  VIL2_PIXEL_FORMAT_FLOAT
  sizeof(double),   //  VIL2_PIXEL_FORMAT_DOUBLE
  sizeof(bool),     //  VIL2_PIXEL_FORMAT_BOOL

  sizeof(int),      //  VIL2_PIXEL_FORMAT_RGB_UNSIGNED_INT
  sizeof(int),      //  VIL2_PIXEL_FORMAT_RGB_SIGNED_INT
  sizeof(short),    //  VIL2_PIXEL_FORMAT_RGB_UNSIGNED_SHORT
  sizeof(short),    //  VIL2_PIXEL_FORMAT_RGB_SIGNED_SHORT
  sizeof(char),     //  VIL2_PIXEL_FORMAT_RGB_BYTE
  sizeof(char),     //  VIL2_PIXEL_FORMAT_RGB_SIGNED_CHAR
  sizeof(float),    //  VIL2_PIXEL_FORMAT_RGB_FLOAT
  sizeof(double),   //  VIL2_PIXEL_FORMAT_RGB_DOUBLE
};




static unsigned num_components[]={
  0,  //  VIL2_PIXEL_FORMAT_UNSIGNED_INT

  1,  //  VIL2_PIXEL_FORMAT_UNSIGNED_INT
  1,  //  VIL2_PIXEL_FORMAT_SIGNED_INT
  1,  //  VIL2_PIXEL_FORMAT_UNSIGNED_SHORT
  1,  //  VIL2_PIXEL_FORMAT_SIGNED_SHORT
  1,  //  VIL2_PIXEL_FORMAT_BYTE
  1,  //  VIL2_PIXEL_FORMAT_SIGNED_CHAR
  1,  //  VIL2_PIXEL_FORMAT_FLOAT
  1,  //  VIL2_PIXEL_FORMAT_DOUBLE
  1,  //  VIL2_PIXEL_FORMAT_BOOL

  3,  //  VIL2_PIXEL_FORMAT_RGB_UNSIGNED_INT
  3,  //  VIL2_PIXEL_FORMAT_RGB_SIGNED_INT
  3,  //  VIL2_PIXEL_FORMAT_RGB_UNSIGNED_SHORT
  3,  //  VIL2_PIXEL_FORMAT_RGB_SIGNED_SHORT
  3,  //  VIL2_PIXEL_FORMAT_RGB_BYTE
  3,  //  VIL2_PIXEL_FORMAT_RGB_SIGNED_CHAR
  3,  //  VIL2_PIXEL_FORMAT_RGB_FLOAT
  3,  //  VIL2_PIXEL_FORMAT_RGB_DOUBLE
};


//: Return the number of bytes used by each component of pixel format f
unsigned vil2_pixel_format_sizeof_components(enum vil2_pixel_format f)
{
  assert (f >= 0 && f < VIL2_PIXEL_FORMAT_ENUM_END);
  return component_size[f];
}
//: Return the number of components in pixel format f
unsigned vil2_pixel_format_num_components(enum vil2_pixel_format f)
{
  assert (f >= 0 && f < VIL2_PIXEL_FORMAT_ENUM_END);
  return num_components[f];
}

//: output a pretty string representing the pixel format.
vcl_ostream & operator << (vcl_ostream &os, vil2_pixel_format f)
{
  switch(f) {
    case VIL2_PIXEL_FORMAT_UNKNOWN:        os << "VIL2_PIXEL_FORMAT=UNKNOWN";  break;

    case VIL2_PIXEL_FORMAT_UNSIGNED_INT:   os << "VIL2_PIXEL_FORMAT=unsigned int";  break;
    case VIL2_PIXEL_FORMAT_SIGNED_INT:     os << "VIL2_PIXEL_FORMAT=signed int";  break;
    case VIL2_PIXEL_FORMAT_UNSIGNED_SHORT: os << "VIL2_PIXEL_FORMAT=unsigned short";  break;
    case VIL2_PIXEL_FORMAT_SIGNED_SHORT:   os << "VIL2_PIXEL_FORMAT=signed short";  break;
    case VIL2_PIXEL_FORMAT_BYTE:           os << "VIL2_PIXEL_FORMAT=vil_byte";  break;
    case VIL2_PIXEL_FORMAT_SIGNED_CHAR:    os << "VIL2_PIXEL_FORMAT=signed char";  break;
    case VIL2_PIXEL_FORMAT_FLOAT:          os << "VIL2_PIXEL_FORMAT=float";  break;
    case VIL2_PIXEL_FORMAT_DOUBLE:         os << "VIL2_PIXEL_FORMAT=double";  break;
    case VIL2_PIXEL_FORMAT_BOOL:           os << "VIL2_PIXEL_FORMAT=bool";  break;

    case VIL2_PIXEL_FORMAT_RGB_SIGNED_INT:     os << "VIL2_PIXEL_FORMAT=vil_rgb<signed int>";  break;
    case VIL2_PIXEL_FORMAT_RGB_UNSIGNED_INT:   os << "VIL2_PIXEL_FORMAT=vil_rgb<unsigned int>";  break;
    case VIL2_PIXEL_FORMAT_RGB_SIGNED_SHORT:   os << "VIL2_PIXEL_FORMAT=vil_rgb<signed short>";  break;
    case VIL2_PIXEL_FORMAT_RGB_UNSIGNED_SHORT: os << "VIL2_PIXEL_FORMAT=vil_rgb<unsigned short>";  break;
    case VIL2_PIXEL_FORMAT_RGB_BYTE:           os << "VIL2_PIXEL_FORMAT=vil_rgb<vil_byte>";  break;
    case VIL2_PIXEL_FORMAT_RGB_SIGNED_CHAR:    os << "VIL2_PIXEL_FORMAT=vil_rgb<signed char>";  break;
    case VIL2_PIXEL_FORMAT_RGB_FLOAT:          os << "VIL2_PIXEL_FORMAT=vil_rgb<float>";  break;
    case VIL2_PIXEL_FORMAT_RGB_DOUBLE:         os << "VIL2_PIXEL_FORMAT=vil_rgb<double>";  break;

    default:  os << "VIL2_PIXEL_FORMAT_INVALID";  break;
  }
  return os;
}

