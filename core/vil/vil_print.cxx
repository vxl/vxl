// This is core/vil/vil_print.cxx
#include "vil_print.h"
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes - Manchester
//
// \verbatim
//  Modifications
//   23 Oct.2003 - Peter Vanroose - Added support for 64-bit int pixels
// \endverbatim

#include <vxl_config.h> // for vxl_uint_32 etc.
#include <vcl_complex.h>
#include <vil/vil_rgb.h>
#include <vil/vil_rgba.h>

//: Explicit overload for bool
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const bool& value, unsigned)
{
  os<<int(value);
}

//: Explicit overload for byte
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vxl_byte& value, unsigned)
{
  os.width(3);
  os<<int(value);
}

//: Explicit overload for signed byte
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vxl_sbyte& value, unsigned)
{
  os.width(3);
  os<<int(value);
}

//: Explicit overload for short
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vxl_int_16& value, unsigned width/*=0*/)
{
  if (width==0) width=5;
  int v=value;
  if (v<0) { v=-v; os<<'-'; } else os<<' ';
  if (v<10 && width > 1)    os<<'0';
  if (v<100 && width > 2)   os<<'0';
  if (v<1000 && width > 3)  os<<'0';
  if (v<10000 && width > 4) os<<'0';
  os<<v;
}

//: Explicit overload for unsigned short
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vxl_uint_16& value, unsigned width/*=0*/)
{
  if (width==0) width=5;
  if (value<10 && width > 1)    os<<'0';
  if (value<100 && width > 2)   os<<'0';
  if (value<1000 && width > 3)  os<<'0';
  if (value<10000 && width > 4) os<<'0';
  os<<value;
}

//: Explicit overload for int
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vxl_int_32& value, unsigned width/*=0*/)
{
  if (width==0) width=8;
  int v=value;
  if (v<0) { v=-v; os<<'-'; } else os<<' ';
  if (v<10 && width > 1)       os<<'0';
  if (v<100 && width > 2)      os<<'0';
  if (v<1000 && width > 3)     os<<'0';
  if (v<10000 && width > 4)    os<<'0';
  if (v<100000 && width > 5)   os<<'0';
  if (v<1000000 && width > 6)  os<<'0';
  if (v<10000000 && width > 7) os<<'0';
  os<<v;
}

//: Explicit overload for unsigned int
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vxl_uint_32& value, unsigned width/*=0*/)
{
  if (width==0) width=8;
  if (value<10 && width > 1)       os<<'0';
  if (value<100 && width > 2)      os<<'0';
  if (value<1000 && width > 3)     os<<'0';
  if (value<10000 && width > 4)    os<<'0';
  if (value<100000 && width > 5)   os<<'0';
  if (value<1000000 && width > 6)  os<<'0';
  if (value<10000000 && width > 7) os<<'0';
  os<<value;
}

#if VXL_HAS_INT_64

//: Explicit overload for unsigned long
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vxl_uint_64& value, unsigned width/*=0*/)
{
  if (width==0) width=8;
#ifdef VCL_VC_6 // IMS. This Hack could be replaced by code which
  os << "****"; // splits the 64bit int and does the right thing.
#else
  if (value<10 && width > 1)       os<<'0';
  if (value<100 && width > 2)      os<<'0';
  if (value<1000 && width > 3)     os<<'0';
  if (value<10000 && width > 4)    os<<'0';
  if (value<100000 && width > 5)   os<<'0';
  if (value<1000000 && width > 6)  os<<'0';
  if (value<10000000 && width > 7) os<<'0';
  os<<value;
#endif
}

//: Explicit overload for long
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vxl_int_64& value, unsigned width/*=0*/)
{
  if (width==0) width=8;
#ifdef VCL_VC_6 // IMS. This Hack could be replaced by code which
  os << "****"; // splits the 64bit int and does the right thing.
#else
  vxl_int_64 v=value;
  if (v<0) { v=-v; os<<'-'; } else os<<' ';
  if (v<10 && width > 1)       os<<'0';
  if (v<100 && width > 2)      os<<'0';
  if (v<1000 && width > 3)     os<<'0';
  if (v<10000 && width > 4)    os<<'0';
  if (v<100000 && width > 5)   os<<'0';
  if (v<1000000 && width > 6)  os<<'0';
  if (v<10000000 && width > 7) os<<'0';
  os<<v;
#endif
}

#endif

//: Explicit overload for float
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const float& value, unsigned)
{
  os<<value;
}

//: Explicit overload for double
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const double& value, unsigned)
{
  os<<value;
}

//: Explicit overload for complex float
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vcl_complex<float>& value, unsigned)
{
  os<<value;
}

//: Explicit overload for complex double
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vcl_complex<double>& value, unsigned)
{
  os<<value;
}

//: Explicit overload of print for rgb<byte>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgb<vxl_byte>& value, unsigned)
{
  int r = int(value.r);
  if (r<10)  os<<'0';
  if (r<100) os<<'0';
  os<<r<<'/';
  int g = int(value.g);
  if (g<10)  os<<'0';
  if (g<100) os<<'0';
  os<<g<<'/';
  int b = int(value.b);
  if (b<10)  os<<'0';
  if (b<100) os<<'0';
  os<<b;
}

//: Explicit overload of print for rgb<sbyte>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, vil_rgb<vxl_sbyte> const& value, unsigned)
{
  int r = int(value.r);
  if (r<0) r=-r,os<<'-'; else os<<'+';
  if (r<10)  os<<'0';
  if (r<100) os<<'0';
  os<<r<<'/';
  int g = int(value.g);
  if (g<0) g=-g,os<<'-'; else os<<'+';
  if (g<10)  os<<'0';
  if (g<100) os<<'0';
  os<<g<<'/';
  int b = int(value.b);
  if (b<0) b=-b,os<<'-'; else os<<'+';
  if (b<10)  os<<'0';
  if (b<100) os<<'0';
  os<<b;
}

//: Explicit overload of print for rgb<short>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgb<vxl_int_16>& value, unsigned width)
{
  vil_print_value(os, value.r, width);
  os<<'/';
  vil_print_value(os, value.g, width);
  os<<'/';
  vil_print_value(os, value.b, width);
}

//: Explicit overload of print for rgb<unsigned short>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgb<vxl_uint_16>& value, unsigned width)
{
  vil_print_value(os, value.r, width);
  os<<'/';
  vil_print_value(os, value.g, width);
  os<<'/';
  vil_print_value(os, value.b, width);
}

//: Explicit overload of print for rgb<int>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgb<vxl_int_32>& value, unsigned width)
{
  vil_print_value(os, value.r, width);
  os<<'/';
  vil_print_value(os, value.g, width);
  os<<'/';
  vil_print_value(os, value.b, width);
}

//: Explicit overload of print for rgb<unsigned int>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgb<vxl_uint_32>& value, unsigned width)
{
  vil_print_value(os, value.r, width);
  os<<'/';
  vil_print_value(os, value.g, width);
  os<<'/';
  vil_print_value(os, value.b, width);
}

#if VXL_HAS_INT_64

//: Explicit overload of print for rgb<long>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgb<vxl_int_64>& value, unsigned width)
{
  vil_print_value(os, value.r, width);
  os<<'/';
  vil_print_value(os, value.g, width);
  os<<'/';
  vil_print_value(os, value.b, width);
}

//: Explicit overload of print for rgb<unsigned long>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgb<vxl_uint_64>& value, unsigned width)
{
  vil_print_value(os, value.r, width);
  os<<'/';
  vil_print_value(os, value.g, width);
  os<<'/';
  vil_print_value(os, value.b, width);
}

#endif

//: Explicit overload of print for rgb<float>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgb<float>& value, unsigned)
{
  os<<value.r<<'/'<<value.g<<'/'<<value.b;
}


//: Explicit overload of print for rgb<double>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgb<double>& value, unsigned)
{
  os<<value.r<<'/'<<value.g<<'/'<<value.b;
}

//: Explicit overload of print for rgba<byte>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgba<vxl_byte>& value, unsigned)
{
  int r = int(value.r);
  if (r<10)  os<<'0';
  if (r<100) os<<'0';
  os<<r<<'/';
  int g = int(value.g);
  if (g<10)  os<<'0';
  if (g<100) os<<'0';
  os<<g<<'/';
  int b = int(value.b);
  if (b<10)  os<<'0';
  if (b<100) os<<'0';
  os<<b<<'/';
  int a = int(value.a);
  if (a<10)  os<<'0';
  if (a<100) os<<'0';
  os<<a;
}

//: Explicit overload of print for rgba<sbyte>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgba<vxl_sbyte>& value, unsigned)
{
  int r = int(value.r);
  if (r<0) r=-r,os<<'-'; else os<<'+';
  if (r<10)  os<<'0';
  if (r<100) os<<'0';
  os<<r<<'/';
  int g = int(value.g);
  if (g<0) g=-g,os<<'-'; else os<<'+';
  if (g<10)  os<<'0';
  if (g<100) os<<'0';
  os<<g<<'/';
  int b = int(value.b);
  if (b<0) b=-b,os<<'-'; else os<<'+';
  if (b<10)  os<<'0';
  if (b<100) os<<'0';
  os<<b<<'/';
  int a = int(value.a);
  if (a<0) a=-a,os<<'-'; else os<<'+';
  if (a<10)  os<<'0';
  if (a<100) os<<'0';
  os<<a;
}

//: Explicit overload of print for rgba<short>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgba<vxl_int_16>& value, unsigned width)
{
  vil_print_value(os, value.r, width);
  os<<'/';
  vil_print_value(os, value.g, width);
  os<<'/';
  vil_print_value(os, value.b, width);
  os<<'/';
  vil_print_value(os, value.a, width);
}

//: Explicit overload of print for rgba<unsigned short>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgba<vxl_uint_16>& value, unsigned width)
{
  vil_print_value(os, value.r, width);
  os<<'/';
  vil_print_value(os, value.g, width);
  os<<'/';
  vil_print_value(os, value.b, width);
  os<<'/';
  vil_print_value(os, value.a, width);
}

//: Explicit overload of print for rgba<int>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgba<vxl_int_32>& value, unsigned width)
{
  vil_print_value(os, value.r, width);
  os<<'/';
  vil_print_value(os, value.g, width);
  os<<'/';
  vil_print_value(os, value.b, width);
  os<<'/';
  vil_print_value(os, value.a, width);
}

//: Explicit overload of print for rgba<unsigned int>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgba<vxl_uint_32>& value, unsigned width)
{
  vil_print_value(os, value.r, width);
  os<<'/';
  vil_print_value(os, value.g, width);
  os<<'/';
  vil_print_value(os, value.b, width);
  os<<'/';
  vil_print_value(os, value.a, width);
}

#if VXL_HAS_INT_64

//: Explicit overload of print for rgba<long>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgba<vxl_int_64>& value, unsigned width)
{
  vil_print_value(os, value.r, width);
  os<<'/';
  vil_print_value(os, value.g, width);
  os<<'/';
  vil_print_value(os, value.b, width);
  os<<'/';
  vil_print_value(os, value.a, width);
}

//: Explicit overload of print for rgba<unsigned long>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgba<vxl_uint_64>& value, unsigned width)
{
  vil_print_value(os, value.r, width);
  os<<'/';
  vil_print_value(os, value.g, width);
  os<<'/';
  vil_print_value(os, value.b, width);
  os<<'/';
  vil_print_value(os, value.a, width);
}

#endif

//: Explicit overload of print for rgba<float>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgba<float>& value, unsigned)
{
  os<<value.r<<'/'<<value.g<<'/'<<value.b<<'/'<<value.a;
}

//: Explicit overload of print for rgba<double>
VCL_DEFINE_SPECIALIZATION
void vil_print_value(vcl_ostream& os, const vil_rgba<double>& value, unsigned)
{
  os<<value.r<<'/'<<value.g<<'/'<<value.b<<'/'<<value.a;
}

void vil_print_all(vcl_ostream& os, vil_image_view_base_sptr const& view)
{
#define docase(T) \
   case T: \
     vil_print_all(os, static_cast<vil_image_view< vil_pixel_format_type_of<T >::type > >(view) );\
     break

  switch ( view->pixel_format() )
  {
#if VXL_HAS_INT_64
    docase( VIL_PIXEL_FORMAT_UINT_64 );
    docase( VIL_PIXEL_FORMAT_INT_64 );
#endif
    docase( VIL_PIXEL_FORMAT_UINT_32 );
    docase( VIL_PIXEL_FORMAT_INT_32 );
    docase( VIL_PIXEL_FORMAT_UINT_16 );
    docase( VIL_PIXEL_FORMAT_INT_16 );
    docase( VIL_PIXEL_FORMAT_BYTE );
    docase( VIL_PIXEL_FORMAT_SBYTE );
    docase( VIL_PIXEL_FORMAT_FLOAT );
    docase( VIL_PIXEL_FORMAT_DOUBLE );
    docase( VIL_PIXEL_FORMAT_BOOL );

#if VXL_HAS_INT_64
    docase( VIL_PIXEL_FORMAT_RGB_UINT_64 );
    docase( VIL_PIXEL_FORMAT_RGB_INT_64 );
#endif
    docase( VIL_PIXEL_FORMAT_RGB_UINT_32 );
    docase( VIL_PIXEL_FORMAT_RGB_INT_32 );
    docase( VIL_PIXEL_FORMAT_RGB_UINT_16 );
    docase( VIL_PIXEL_FORMAT_RGB_INT_16 );
    docase( VIL_PIXEL_FORMAT_RGB_BYTE );
    docase( VIL_PIXEL_FORMAT_RGB_SBYTE );
    docase( VIL_PIXEL_FORMAT_RGB_FLOAT );
    docase( VIL_PIXEL_FORMAT_RGB_DOUBLE );

#if VXL_HAS_INT_64
    docase( VIL_PIXEL_FORMAT_RGBA_UINT_64 );
    docase( VIL_PIXEL_FORMAT_RGBA_INT_64 );
#endif
    docase( VIL_PIXEL_FORMAT_RGBA_UINT_32 );
    docase( VIL_PIXEL_FORMAT_RGBA_INT_32 );
    docase( VIL_PIXEL_FORMAT_RGBA_UINT_16 );
    docase( VIL_PIXEL_FORMAT_RGBA_INT_16 );
    docase( VIL_PIXEL_FORMAT_RGBA_BYTE );
    docase( VIL_PIXEL_FORMAT_RGBA_SBYTE );
    docase( VIL_PIXEL_FORMAT_RGBA_FLOAT );
    docase( VIL_PIXEL_FORMAT_RGBA_DOUBLE );

    docase( VIL_PIXEL_FORMAT_COMPLEX_FLOAT );
    docase( VIL_PIXEL_FORMAT_COMPLEX_DOUBLE );

    default: ;
  }
#undef docase
}
