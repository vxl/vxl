// This is core/vil1/vil1_clamp.h
#ifndef vil1_clamp_h_
#define vil1_clamp_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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
vil1_image vil1_clamp(vil1_image src, double range_min, double range_max);

//: Convenience templated functions for clamping of a single pixel.
//    (vxl_byte)vil1_clamp_pixel(g, 0, 255);
//    (vil1_rgb<vxl_byte>)vil1_clamp_pixel(rgb, 0, 255);
template <class V>
inline
V vil1_clamp_pixel(V const& b, double range_min, double range_max)
{
  return (b < V(range_min) ? V(range_min) : (b > V(range_max) ? V(range_max) : b));
}

// capes@robots : The functions below are deprecated.
// Use the adaptor style or the vil1_clamp_pixel functions instead.
#if 0
//:
// Default behaviour just returns value. Clamping of double to return byte is
// also defined here. Other clamps, such as vil1_rgb<double> to vil1_rgb<byte>
// may be defined in the appropriate places.
//
#if !defined(VCL_SGI_CC)
template <class V, class U>
inline
U vil1_clamp(const V& b, U*)
{
   return U(b);
}
#endif

inline
unsigned char vil1_clamp(const float& d, unsigned char *)
{
  if (d > 255.0)
    return 255;
  else if (d < 0.0)
    return 0;
  else
    return (unsigned char)d;
}

inline
unsigned char vil1_clamp(const double& d, unsigned char *)
{
  if (d > 255.0)
    return 255;
  else if (d < 0.0)
    return 0;
  else
    return (unsigned char)d;
}
#endif

#endif // vil1_clamp_h_
