// This is vxl/vil/vil_clamp.h
#ifndef vil_clamp_h_
#define vil_clamp_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Templated vil_clamping functions
//
// To ensure value is in
// a range appropriate for the data type (e.g. 0-255 for bytes).
//
// \author Andrew Fitzgibbon and David Capel

#include <vil/vil_image.h>

//: Adaptor which returns a vil_image with pixel components clamped to given range.
// Fits nicely into the functional composition and lazy evaluation scheme
// provided by the other vil adaptors.
// Old code can be implemented in the new style as
//   vil_image_as_byte(vil_clamp(img, 0, 255));
//   vil_image_as_rgb_byte(vil_clamp(img, 0, 255));
vil_image vil_clamp(vil_image src, double range_min, double range_max);

//: Convenience templated functions for clamping of a single pixel.
//    (vil_byte)vil_clamp_pixel(g, 0, 255);
//    (vil_rgb<vil_byte>)vil_clamp_pixel(rgb, 0, 255);
template <class V>
inline
V vil_clamp_pixel(V const& b, double range_min, double range_max)
{
  return (b < V(range_min) ? V(range_min) : (b > V(range_max) ? V(range_max) : b));
}

// capes@robots : The functions below are deprecated.
// Use the adaptor style or the vil_clamp_pixel functions instead.
#if 0
//:
// Default behaviour just returns value. Clamping of double to return byte is
// also defined here. Other clamps, such as vil_rgb<double> to vil_rgb<byte>
// may be defined in the appropriate places.
//
#if !defined(VCL_SGI_CC)
template <class V, class U>
inline
U vil_clamp(const V& b, U*)
{
   return U(b);
}
#endif

inline
unsigned char vil_clamp(const float& d, unsigned char *)
{
  if (d > 255.0)
    return 255;
  else if (d < 0.0)
    return 0;
  else
    return (unsigned char)d;
}

inline
unsigned char vil_clamp(const double& d, unsigned char *)
{
  if (d > 255.0)
    return 255;
  else if (d < 0.0)
    return 0;
  else
    return (unsigned char)d;
}
#endif

#endif // vil_clamp_h_
