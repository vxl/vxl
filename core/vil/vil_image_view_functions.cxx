// This is mul/vil2/vil2_image_view_functions.cxx
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view_functions.h>
#include <vil/vil_rgb.h>

//: Explicit overload of print for rgb
void vil2_print_value(vcl_ostream& os, const vil_rgb<unsigned char>& value)
{
  int r = int(value.r);
  if (r<10)  os<<"0";
  if (r<100) os<<"0";
  os<<r<<"-";
  int g = int(value.g);
  if (g<10)  os<<"0";
  if (g<100) os<<"0";
  os<<g<<"-";
  int b = int(value.b);
  if (b<10)  os<<"0";
  if (b<100) os<<"0";
  os<<b;
}

//: Explicit overload of print for rgb
void vil2_print_value(vcl_ostream& os, const vil_rgb<float>& value)
{
  int r = int(value.r);
  if (r<10)  os<<"0";
  if (r<100) os<<"0";
  os<<r<<"-";
  int g = int(value.g);
  if (g<10)  os<<"0";
  if (g<100) os<<"0";
  os<<g<<"-";
  int b = int(value.b);
  if (b<10)  os<<"0";
  if (b<100) os<<"0";
  os<<b;
}
