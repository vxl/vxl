// This is core/vil/vil_pixel_format.cxx
#include "vil_pixel_format.h"
//:
// \file
// \author Ian Scott.
//
// \verbatim
//  Modifications
//   23 Oct.2003 - Peter Vanroose - Added support for 64-bit int pixels
// \endvarbatim

#include <vcl_cassert.h>
#include <vcl_complex.h>

static unsigned component_size[]={
  0,                   //  VIL_PIXEL_FORMAT_UNKNOWN

#if VXL_HAS_INT_64
  sizeof(vxl_uint_64), //  VIL_PIXEL_FORMAT_UINT_64
  sizeof(vxl_int_64),  //  VIL_PIXEL_FORMAT_INT_64
#else
  0,                   //  VIL_PIXEL_FORMAT_UINT_64
  0,                   //  VIL_PIXEL_FORMAT_INT_64
#endif
  sizeof(vxl_uint_32), //  VIL_PIXEL_FORMAT_UINT_32
  sizeof(vxl_int_32),  //  VIL_PIXEL_FORMAT_INT_32
  sizeof(vxl_uint_16), //  VIL_PIXEL_FORMAT_UINT_16
  sizeof(vxl_int_16),  //  VIL_PIXEL_FORMAT_INT_16
  sizeof(vxl_byte),    //  VIL_PIXEL_FORMAT_BYTE
  sizeof(vxl_sbyte),   //  VIL_PIXEL_FORMAT_SBYTE
  sizeof(float),       //  VIL_PIXEL_FORMAT_FLOAT
  sizeof(double),      //  VIL_PIXEL_FORMAT_DOUBLE
  0,                   //  VIL_PIXEL_FORMAT_LONG_DOUBLE
  sizeof(bool),        //  VIL_PIXEL_FORMAT_BOOL

#if VXL_HAS_INT_64
  sizeof(vxl_uint_64), //  VIL_PIXEL_FORMAT_RGB_UINT_64
  sizeof(vxl_int_64),  //  VIL_PIXEL_FORMAT_RGB_INT_64
#else
  0,                   //  VIL_PIXEL_FORMAT_RGB_UINT_64
  0,                   //  VIL_PIXEL_FORMAT_RGB_INT_64
#endif
  sizeof(vxl_uint_32), //  VIL_PIXEL_FORMAT_RGB_UINT_32
  sizeof(vxl_int_32),  //  VIL_PIXEL_FORMAT_RGB_INT_32
  sizeof(vxl_uint_16), //  VIL_PIXEL_FORMAT_RGB_UINT_16
  sizeof(vxl_int_16),  //  VIL_PIXEL_FORMAT_RGB_INT_16
  sizeof(vxl_byte),    //  VIL_PIXEL_FORMAT_RGB_BYTE
  sizeof(vxl_sbyte),   //  VIL_PIXEL_FORMAT_RGB_SBYTE
  sizeof(float),       //  VIL_PIXEL_FORMAT_RGB_FLOAT
  sizeof(double),      //  VIL_PIXEL_FORMAT_RGB_DOUBLE
  0,                   //  VIL_PIXEL_FORMAT_RGB_LONG_DOUBLE

#if VXL_HAS_INT_64
  sizeof(vxl_uint_64), //  VIL_PIXEL_FORMAT_RGBA_UINT_64
  sizeof(vxl_int_64),  //  VIL_PIXEL_FORMAT_RGBA_INT_64
#else
  0,                   //  VIL_PIXEL_FORMAT_RGBA_UINT_64
  0,                   //  VIL_PIXEL_FORMAT_RGBA_INT_64
#endif
  sizeof(vxl_uint_32), //  VIL_PIXEL_FORMAT_RGBA_UINT_32
  sizeof(vxl_int_32),  //  VIL_PIXEL_FORMAT_RGBA_INT_32
  sizeof(vxl_uint_16), //  VIL_PIXEL_FORMAT_RGBA_UINT_16
  sizeof(vxl_int_16),  //  VIL_PIXEL_FORMAT_RGBA_INT_16
  sizeof(vxl_byte),    //  VIL_PIXEL_FORMAT_RGBA_BYTE
  sizeof(vxl_sbyte),   //  VIL_PIXEL_FORMAT_RGBA_SBYTE
  sizeof(float),       //  VIL_PIXEL_FORMAT_RGBA_FLOAT
  0,                   //  VIL_PIXEL_FORMAT_RGBA_LONG_DOUBLE

  sizeof(vcl_complex<float>),   // VIL_PIXEL_FORMAT_COMPLEX_FLOAT
  sizeof(vcl_complex<double>),  // VIL_PIXEL_FORMAT_COMPLEX_DOUBLE
};


static unsigned num_components[]={
  0,  //  VIL_PIXEL_FORMAT_UNKNOWN

#if VXL_HAS_INT_64
  1,  //  VIL_PIXEL_FORMAT_UINT_64
  1,  //  VIL_PIXEL_FORMAT_INT_64
#else
  0,  //  VIL_PIXEL_FORMAT_UINT_64
  0,  //  VIL_PIXEL_FORMAT_INT_64
#endif
  1,  //  VIL_PIXEL_FORMAT_UINT_32
  1,  //  VIL_PIXEL_FORMAT_INT_32
  1,  //  VIL_PIXEL_FORMAT_UINT_16
  1,  //  VIL_PIXEL_FORMAT_INT_16
  1,  //  VIL_PIXEL_FORMAT_BYTE
  1,  //  VIL_PIXEL_FORMAT_SBYTE
  1,  //  VIL_PIXEL_FORMAT_FLOAT
  1,  //  VIL_PIXEL_FORMAT_DOUBLE
  0,  //  VIL_PIXEL_FORMAT_LONG_DOUBLE
  1,  //  VIL_PIXEL_FORMAT_BOOL

#if VXL_HAS_INT_64
  3,  //  VIL_PIXEL_FORMAT_RGB_UINT_64
  3,  //  VIL_PIXEL_FORMAT_RGB_INT_64
#else
  0,  //  VIL_PIXEL_FORMAT_RGB_UINT_64
  0,  //  VIL_PIXEL_FORMAT_RGB_INT_64
#endif
  3,  //  VIL_PIXEL_FORMAT_RGB_UINT_32
  3,  //  VIL_PIXEL_FORMAT_RGB_INT_32
  3,  //  VIL_PIXEL_FORMAT_RGB_UINT_16
  3,  //  VIL_PIXEL_FORMAT_RGB_INT_16
  3,  //  VIL_PIXEL_FORMAT_RGB_BYTE
  3,  //  VIL_PIXEL_FORMAT_RGB_SBYTE
  3,  //  VIL_PIXEL_FORMAT_RGB_FLOAT
  3,  //  VIL_PIXEL_FORMAT_RGB_DOUBLE
  0,  //  VIL_PIXEL_FORMAT_RGB_LONG_DOUBLE

#if VXL_HAS_INT_64
  4,  //  VIL_PIXEL_FORMAT_RGBA_UINT_64
  4,  //  VIL_PIXEL_FORMAT_RGBA_INT_64
#else
  0,  //  VIL_PIXEL_FORMAT_RGBA_UINT_64
  0,  //  VIL_PIXEL_FORMAT_RGBA_INT_64
#endif
  4,  //  VIL_PIXEL_FORMAT_RGBA_UINT_32
  4,  //  VIL_PIXEL_FORMAT_RGBA_INT_32
  4,  //  VIL_PIXEL_FORMAT_RGBA_UINT_16
  4,  //  VIL_PIXEL_FORMAT_RGBA_INT_16
  4,  //  VIL_PIXEL_FORMAT_RGBA_BYTE
  4,  //  VIL_PIXEL_FORMAT_RGBA_SBYTE
  4,  //  VIL_PIXEL_FORMAT_RGBA_FLOAT
  4,  //  VIL_PIXEL_FORMAT_RGBA_DOUBLE
  0,  //  VIL_PIXEL_FORMAT_RGBA_LONG_DOUBLE

  1,  //  VIL_PIXEL_FORMAT_COMPLEX_FLOAT
  1,  //  VIL_PIXEL_FORMAT_COMPLEX_DOUBLE
};


static vil_pixel_format component_format[]={
  VIL_PIXEL_FORMAT_UNKNOWN,  //  VIL_PIXEL_FORMAT_UNKNOWN

#if VXL_HAS_INT_64
  VIL_PIXEL_FORMAT_UINT_64,  //  VIL_PIXEL_FORMAT_UINT_64
  VIL_PIXEL_FORMAT_INT_64,   //  VIL_PIXEL_FORMAT_INT_64
#else
  VIL_PIXEL_FORMAT_UNKNOWN,  //  VIL_PIXEL_FORMAT_UINT_64
  VIL_PIXEL_FORMAT_UNKNOWN,  //  VIL_PIXEL_FORMAT_INT_64
#endif
  VIL_PIXEL_FORMAT_UINT_32,  //  VIL_PIXEL_FORMAT_UINT_32
  VIL_PIXEL_FORMAT_INT_32,   //  VIL_PIXEL_FORMAT_INT_32
  VIL_PIXEL_FORMAT_UINT_16,  //  VIL_PIXEL_FORMAT_UINT_16
  VIL_PIXEL_FORMAT_INT_16,   //  VIL_PIXEL_FORMAT_INT_16
  VIL_PIXEL_FORMAT_BYTE,     //  VIL_PIXEL_FORMAT_BYTE
  VIL_PIXEL_FORMAT_SBYTE,    //  VIL_PIXEL_FORMAT_SBYTE
  VIL_PIXEL_FORMAT_FLOAT,    //  VIL_PIXEL_FORMAT_FLOAT
  VIL_PIXEL_FORMAT_DOUBLE,   //  VIL_PIXEL_FORMAT_DOUBLE
  VIL_PIXEL_FORMAT_UNKNOWN,  //  VIL_PIXEL_FORMAT_LONG_DOUBLE
  VIL_PIXEL_FORMAT_BOOL,     //  VIL_PIXEL_FORMAT_BOOL

#if VXL_HAS_INT_64
  VIL_PIXEL_FORMAT_UINT_64,  //  VIL_PIXEL_FORMAT_RGB_UINT_64
  VIL_PIXEL_FORMAT_INT_64,   //  VIL_PIXEL_FORMAT_RGB_INT_64
#else
  VIL_PIXEL_FORMAT_UNKNOWN,  //  VIL_PIXEL_FORMAT_RGB_UINT_64
  VIL_PIXEL_FORMAT_UNKNOWN,  //  VIL_PIXEL_FORMAT_RGB_INT_64
#endif
  VIL_PIXEL_FORMAT_UINT_32,  //  VIL_PIXEL_FORMAT_RGB_UINT_32
  VIL_PIXEL_FORMAT_INT_32,   //  VIL_PIXEL_FORMAT_RGB_INT_32
  VIL_PIXEL_FORMAT_UINT_16,  //  VIL_PIXEL_FORMAT_RGB_UINT_16
  VIL_PIXEL_FORMAT_INT_16,   //  VIL_PIXEL_FORMAT_RGB_INT_16
  VIL_PIXEL_FORMAT_BYTE,     //  VIL_PIXEL_FORMAT_RGB_BYTE
  VIL_PIXEL_FORMAT_SBYTE,    //  VIL_PIXEL_FORMAT_RGB_SBYTE
  VIL_PIXEL_FORMAT_FLOAT,    //  VIL_PIXEL_FORMAT_RGB_FLOAT
  VIL_PIXEL_FORMAT_DOUBLE,   //  VIL_PIXEL_FORMAT_RGB_DOUBLE
  VIL_PIXEL_FORMAT_UNKNOWN,  //  VIL_PIXEL_FORMAT_RGB_LONG_DOUBLE

#if VXL_HAS_INT_64
  VIL_PIXEL_FORMAT_UINT_64,  //  VIL_PIXEL_FORMAT_RGBA_UINT_64
  VIL_PIXEL_FORMAT_INT_64,   //  VIL_PIXEL_FORMAT_RGBA_INT_64
#else
  VIL_PIXEL_FORMAT_UNKNOWN,  //  VIL_PIXEL_FORMAT_RGBA_UINT_64
  VIL_PIXEL_FORMAT_UNKNOWN,  //  VIL_PIXEL_FORMAT_RGBA_INT_64
#endif
  VIL_PIXEL_FORMAT_UINT_32,  //  VIL_PIXEL_FORMAT_RGBA_UINT_32
  VIL_PIXEL_FORMAT_INT_32,   //  VIL_PIXEL_FORMAT_RGBA_INT_32
  VIL_PIXEL_FORMAT_UINT_16,  //  VIL_PIXEL_FORMAT_RGBA_UINT_16
  VIL_PIXEL_FORMAT_INT_16,   //  VIL_PIXEL_FORMAT_RGBA_INT_16
  VIL_PIXEL_FORMAT_BYTE,     //  VIL_PIXEL_FORMAT_RGBA_BYTE
  VIL_PIXEL_FORMAT_SBYTE,    //  VIL_PIXEL_FORMAT_RGBA_SBYTE
  VIL_PIXEL_FORMAT_FLOAT,    //  VIL_PIXEL_FORMAT_RGBA_FLOAT
  VIL_PIXEL_FORMAT_DOUBLE,   //  VIL_PIXEL_FORMAT_RGBA_DOUBLE
  VIL_PIXEL_FORMAT_UNKNOWN,  //  VIL_PIXEL_FORMAT_RGBA_LONG_DOUBLE

  VIL_PIXEL_FORMAT_COMPLEX_FLOAT, //  VIL_PIXEL_FORMAT_COMPLEX_FLOAT
  VIL_PIXEL_FORMAT_COMPLEX_DOUBLE,//  VIL_PIXEL_FORMAT_COMPLEX_DOUBLE
};


//: Return the number of bytes used by each component of pixel format f
unsigned vil_pixel_format_sizeof_components(enum vil_pixel_format f)
{
  assert (f >= VIL_PIXEL_FORMAT_UNKNOWN && f < VIL_PIXEL_FORMAT_ENUM_END);
  return component_size[f];
}

//: Return the number of components in pixel format f
unsigned vil_pixel_format_num_components(enum vil_pixel_format f)
{
  assert (f >= VIL_PIXEL_FORMAT_UNKNOWN && f < VIL_PIXEL_FORMAT_ENUM_END);
  return num_components[f];
}


//: Return the number of components in pixel format f
vil_pixel_format vil_pixel_format_component_format(enum vil_pixel_format f)
{
  assert (f >= VIL_PIXEL_FORMAT_UNKNOWN && f < VIL_PIXEL_FORMAT_ENUM_END);
  return component_format[f];
}


//: output a pretty string representing the pixel format.
vcl_ostream & operator << (vcl_ostream &os, vil_pixel_format f)
{
  switch(f) {
    case VIL_PIXEL_FORMAT_UNKNOWN: os << "VIL_PIXEL_FORMAT_UNKNOWN";  break;

#if VXL_HAS_INT_64
    case VIL_PIXEL_FORMAT_UINT_64: os << "vxl_uint_64";  break;
    case VIL_PIXEL_FORMAT_INT_64:  os << "vxl_int_64";  break;
#endif
    case VIL_PIXEL_FORMAT_UINT_32: os << "vxl_uint_32";  break;
    case VIL_PIXEL_FORMAT_INT_32:  os << "vxl_int_32";  break;
    case VIL_PIXEL_FORMAT_UINT_16: os << "vxl_uint_16";  break;
    case VIL_PIXEL_FORMAT_INT_16:  os << "vxl_int_16";  break;
    case VIL_PIXEL_FORMAT_BYTE:    os << "vxl_byte";  break;
    case VIL_PIXEL_FORMAT_SBYTE:   os << "vxl_sbyte";  break;
    case VIL_PIXEL_FORMAT_FLOAT:   os << "float";  break;
    case VIL_PIXEL_FORMAT_DOUBLE:  os << "double";  break;
    case VIL_PIXEL_FORMAT_BOOL:    os << "bool";  break;

#if VXL_HAS_INT_64
    case VIL_PIXEL_FORMAT_RGB_INT_64:  os << "vil_rgb<vxl_int_64>";  break;
    case VIL_PIXEL_FORMAT_RGB_UINT_64: os << "vil_rgb<vxl_uint_64>";  break;
#endif
    case VIL_PIXEL_FORMAT_RGB_INT_32:  os << "vil_rgb<vxl_int_32>";  break;
    case VIL_PIXEL_FORMAT_RGB_UINT_32: os << "vil_rgb<vxl_uint_32>";  break;
    case VIL_PIXEL_FORMAT_RGB_INT_16:  os << "vil_rgb<vxl_int_16>";  break;
    case VIL_PIXEL_FORMAT_RGB_UINT_16: os << "vil_rgb<vxl_uint_16>";  break;
    case VIL_PIXEL_FORMAT_RGB_BYTE:    os << "vil_rgb<vxl_byte>";  break;
    case VIL_PIXEL_FORMAT_RGB_SBYTE:   os << "vil_rgb<vxl_sbyte>";  break;
    case VIL_PIXEL_FORMAT_RGB_FLOAT:   os << "vil_rgb<float>";  break;
    case VIL_PIXEL_FORMAT_RGB_DOUBLE:  os << "vil_rgb<double>";  break;

#if VXL_HAS_INT_64
    case VIL_PIXEL_FORMAT_RGBA_INT_64:  os << "vil_rgba<vxl_int_64>";  break;
    case VIL_PIXEL_FORMAT_RGBA_UINT_64: os << "vil_rgba<vxl_uint_64>";  break;
#endif
    case VIL_PIXEL_FORMAT_RGBA_INT_32:  os << "vil_rgba<vxl_int_32>";  break;
    case VIL_PIXEL_FORMAT_RGBA_UINT_32: os << "vil_rgba<vxl_uint_32>";  break;
    case VIL_PIXEL_FORMAT_RGBA_INT_16:  os << "vil_rgba<vxl_int_16>";  break;
    case VIL_PIXEL_FORMAT_RGBA_UINT_16: os << "vil_rgba<vxl_uint_16>";  break;
    case VIL_PIXEL_FORMAT_RGBA_BYTE:    os << "vil_rgba<vxl_byte>";  break;
    case VIL_PIXEL_FORMAT_RGBA_SBYTE:   os << "vil_rgba<vxl_sbyte>";  break;
    case VIL_PIXEL_FORMAT_RGBA_FLOAT:   os << "vil_rgba<float>";  break;
    case VIL_PIXEL_FORMAT_RGBA_DOUBLE:  os << "vil_rgba<double>";  break;

    case VIL_PIXEL_FORMAT_COMPLEX_FLOAT:   os << "complex<float>";  break;
    case VIL_PIXEL_FORMAT_COMPLEX_DOUBLE:  os << "complex<double>";  break;

    default:  os << "VIL_PIXEL_FORMAT_INVALID";  break;
  }
  return os;
}

