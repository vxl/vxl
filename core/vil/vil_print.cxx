// This is mul/vil2/vil2_print.cxx
#include "vil2_print.h"
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes - Manchester

#include <vxl_config.h> // for vxl_uint_32 etc.
#include <vil/vil_rgb.h>

//: Explicit overload for bool
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const bool& value)
{
  os<<int(value);
}

//: Explicit overload for unsigned char
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vxl_byte& value)
{
  os.width(3);
  os<<int(value);
}

//: Explicit overload for unsigned char
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vxl_sbyte& value)
{
  os.width(3);
  os<<int(value);
}

//: Explicit overload for unsigned short
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vxl_int_16& value)
{
  int v=value;
  if (v<0) { v=-v; os<<'-'; } else os<<' ';
  if (v<10)    os<<'0';
  if (v<100)   os<<'0';
  if (v<1000)  os<<'0';
  if (v<10000) os<<'0';
  os<<v;
}

//: Explicit overload for unsigned short
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vxl_uint_16& value)
{
  if (value<10)    os<<'0';
  if (value<100)   os<<'0';
  if (value<1000)  os<<'0';
  if (value<10000) os<<'0';
  os<<value;
}

//: Explicit overload for unsigned short
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vxl_uint_32& value)
{
  if (value<10)       os<<'0';
  if (value<100)      os<<'0';
  if (value<1000)     os<<'0';
  if (value<10000)    os<<'0';
  if (value<100000)   os<<'0';
  if (value<1000000)  os<<'0';
  if (value<10000000) os<<'0';
  os<<value;
}

//: Explicit overload for unsigned short
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vxl_int_32& value)
{
  int v=value;
  if (v<0) { v=-v; os<<'-'; } else os<<' ';
  if (v<10)       os<<'0';
  if (v<100)      os<<'0';
  if (v<1000)     os<<'0';
  if (v<10000)    os<<'0';
  if (v<100000)   os<<'0';
  if (v<1000000)  os<<'0';
  if (v<10000000) os<<'0';
  os<<v;
}

//: Explicit overload for float
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const float& value)
{
  os<<value;
}

//: Explicit overload for float
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const double& value)
{
  os<<value;
}

//: Explicit overload of print for rgb<ubyte>
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vil_rgb<vxl_byte>& value)
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

//: Explicit overload of print for rgb<ubyte>
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vil_rgb<vxl_int_16>& value)
{
  vil2_print_value(os, value.r);
  os<<'/';
  vil2_print_value(os, value.g);
  os<<'/';
  vil2_print_value(os, value.b);
}

//: Explicit overload of print for rgb<ubyte>
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vil_rgb<vxl_uint_16>& value)
{
  vil2_print_value(os, value.r);
  os<<'/';
  vil2_print_value(os, value.g);
  os<<'/';
  vil2_print_value(os, value.b);
}

//: Explicit overload of print for rgb<ubyte>
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vil_rgb<vxl_int_32>& value)
{
  vil2_print_value(os, value.r);
  os<<'/';
  vil2_print_value(os, value.g);
  os<<'/';
  vil2_print_value(os, value.b);
}

//: Explicit overload of print for rgb<ubyte>
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vil_rgb<vxl_uint_32>& value)
{
  vil2_print_value(os, value.r);
  os<<'/';
  vil2_print_value(os, value.g);
  os<<'/';
  vil2_print_value(os, value.b);
}


//: Explicit overload of print for rgb<float>
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vil_rgb<float>& value)
{
  os<<value.r<<'/'<<value.g<<'/'<<value.b;
}


//: Explicit overload of print for rgb<float>
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vil_rgb<double>& value)
{
  os<<value.r<<'/'<<value.g<<'/'<<value.b;
}

//: Explicit overload of print for rgb<ubyte>
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vil_rgba<vxl_byte>& value)
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

//: Explicit overload of print for rgb<ubyte>
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vil_rgba<float>& value)
{
  os<<value.r<<'/';
  os<<value.g<<'/';
  os<<value.b<<'/';
  os<<value.a;
}


