// This is mul/vil2/vil2_image_view_functions.cxx
#include "vil2_image_view_functions.h"
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes and Ian Scott - Manchester

#include <vil/vil_rgb.h>
#include <vil2/vil2_image_data.h>

//: Explicit overload for bool
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const bool& value)
{
  os<<int(value);
}

//: Explicit overload for unsigned char
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const unsigned char& value)
{
  os.width(3);
  os<<int(value);
}

//: Explicit overload for unsigned short
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const short& value)
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
void vil2_print_value(vcl_ostream& os, const unsigned short& value)
{
  if (value<10)    os<<'0';
  if (value<100)   os<<'0';
  if (value<1000)  os<<'0';
  if (value<10000) os<<'0';
  os<<value;
}

//: Explicit overload for unsigned short
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const unsigned int& value)
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
void vil2_print_value(vcl_ostream& os, const int& value)
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

//: Explicit overload of print for rgb<ubyte>
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vil_rgb<vil2_byte>& value)
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

//: Explicit overload of print for rgb<ushort>
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vil_rgb<vxl_uint_16>& value)
{
  vil2_print_value(os,value.r); os<<'/';
  vil2_print_value(os,value.g); os<<'/';
  vil2_print_value(os,value.b);
}

//: Explicit overload of print for rgb<float>
VCL_DEFINE_SPECIALIZATION
void vil2_print_value(vcl_ostream& os, const vil_rgb<float>& value)
{
  os<<value.r<<'/'<<value.g<<'/'<<value.b;
}

//: Compute minimum and maximum values over view
VCL_DEFINE_SPECIALIZATION
void vil2_value_range(vil_rgb<vil2_byte>& min_value, vil_rgb<vil2_byte>& max_value,
                      const vil2_image_view<vil_rgb<vil2_byte> >& rgb_view)
{
  vil2_image_view<vil2_byte> plane_view = vil2_view_as_planes(rgb_view);
  // Get range for each plane in turn
  vil2_value_range(min_value.r,max_value.r,plane_view.plane(0));
  vil2_value_range(min_value.g,max_value.g,plane_view.plane(1));
  vil2_value_range(min_value.b,max_value.b,plane_view.plane(2));
}

//: Compute minimum and maximum values over view
VCL_DEFINE_SPECIALIZATION
void vil2_value_range(vil_rgb<float>& min_value, vil_rgb<float>& max_value,
                      const vil2_image_view<vil_rgb<float> >& rgb_view)
{
  vil2_image_view<float> plane_view = vil2_view_as_planes(rgb_view);
  // Get range for each plane in turn
  vil2_value_range(min_value.r,max_value.r,plane_view.plane(0));
  vil2_value_range(min_value.g,max_value.g,plane_view.plane(1));
  vil2_value_range(min_value.b,max_value.b,plane_view.plane(2));
}

