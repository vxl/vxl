// This is mul/vil2/vil2_image_view_functions.cxx
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes and Ian Scott - Manchester

#include <vil/vil_rgb.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil2/vil2_image_data.h>

//: Explicit overload for unsigned char
void vil2_print_value(vcl_ostream& os, const unsigned char& value)
{
  os.width(3);
  os<<int(value);
}

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

//: Compute minimum and maximum values over view
void vil2_value_range(vil_rgb<unsigned char>& min_value, vil_rgb<unsigned char>& max_value,
                      const vil2_image_view<vil_rgb<unsigned char> >& rgb_view)
{
  vil2_image_view<unsigned char> plane_view = vil2_view_as_planes(rgb_view);
  // Get range for each plane in turn
  vil2_value_range(min_value.r,max_value.r,plane_view.plane(0));
  vil2_value_range(min_value.g,max_value.g,plane_view.plane(1));
  vil2_value_range(min_value.b,max_value.b,plane_view.plane(2));
}

//: Compute minimum and maximum values over view
void vil2_value_range(vil_rgb<float>& min_value, vil_rgb<float>& max_value,
                      const vil2_image_view<vil_rgb<float> >& rgb_view)
{
  vil2_image_view<float> plane_view = vil2_view_as_planes(rgb_view);
  // Get range for each plane in turn
  vil2_value_range(min_value.r,max_value.r,plane_view.plane(0));
  vil2_value_range(min_value.g,max_value.g,plane_view.plane(1));
  vil2_value_range(min_value.b,max_value.b,plane_view.plane(2));
}


