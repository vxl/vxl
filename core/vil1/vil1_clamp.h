// This is core/vil1/vil1_clamp.h
#ifndef vil1_clamp_h_
#define vil1_clamp_h_
//:
// \file
// \brief Templated vil1_clamping functions
//
// To ensure value is in
// a range appropriate for the data type (e.g. 0-255 for bytes).
//
// \author Andrew Fitzgibbon and David Capel

#include <vil1/vil1_image.h>

//: Adaptor which returns a vil1_image with pixel components clamped to given range.
// Fits nicely into the functional composition and lazy evaluation scheme
// provided by the other vil1 adaptors.
// Old code can be implemented in the new style as
//   vil1_image_as_byte(vil1_clamp(img, 0, 255));
//   vil1_image_as_rgb_byte(vil1_clamp(img, 0, 255));
vil1_image vil1_clamp(const vil1_image& src, double range_min, double range_max);

//: Convenience templated functions for clamping of a single pixel.
//    (vxl_byte)vil1_clamp_pixel(g, 0, 255);
//    (vil1_rgb<vxl_byte>)vil1_clamp_pixel(rgb, 0, 255);
template <class V>
inline
V vil1_clamp_pixel(V const& b, double range_min, double range_max)
{
  return (b < V(range_min) ? V(range_min) : (b > V(range_max) ? V(range_max) : b));
}


#endif // vil1_clamp_h_
