// This is mul/vil2/vil2_image_view_functions.cxx
#include "vil2_image_view_functions.h"
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes and Ian Scott - Manchester

#include <vxl_config.h> // for vxl_uint_32 etc.
#include <vil/vil_rgb.h>
#include <vil2/vil2_plane.h>



//: Compute minimum and maximum values over view
VCL_DEFINE_SPECIALIZATION
void vil2_value_range(vil_rgb<vxl_byte>& min_value, vil_rgb<vxl_byte>& max_value,
                      const vil2_image_view<vil_rgb<vxl_byte> >& rgb_view)
{
  vil2_image_view<vxl_byte> plane_view = vil2_view_as_planes(rgb_view);
  // Get range for each plane in turn
  vil2_value_range(min_value.r,max_value.r,vil2_plane(plane_view,0));
  vil2_value_range(min_value.g,max_value.g,vil2_plane(plane_view,1));
  vil2_value_range(min_value.b,max_value.b,vil2_plane(plane_view,2));
}

//: Compute minimum and maximum values over view
VCL_DEFINE_SPECIALIZATION
void vil2_value_range(vil_rgb<float>& min_value, vil_rgb<float>& max_value,
                      const vil2_image_view<vil_rgb<float> >& rgb_view)
{
  vil2_image_view<float> plane_view = vil2_view_as_planes(rgb_view);
  // Get range for each plane in turn
  vil2_value_range(min_value.r,max_value.r,vil2_plane(plane_view,0));
  vil2_value_range(min_value.g,max_value.g,vil2_plane(plane_view,1));
  vil2_value_range(min_value.b,max_value.b,vil2_plane(plane_view,2));
}

