// This is vxl/vil/vil_clamp.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// vil_clamp
// Author: David Capel, Oxford RRG
// Created: 14 Aug 98
//
//-----------------------------------------------------------------------------

#include "vil_clamp.h"
#include <vil/vil_clamp_image_impl.h>

vil_image vil_clamp(vil_image src, double range_min, double range_max) {
  return new vil_clamp_image_impl(src, range_min, range_max);
}

#undef VIL_CLAMP_PIXEL_INSTANTIATE
#define VIL_CLAMP_PIXEL_INSTANTIATE(T) \
VCL_INSTANTIATE_INLINE(T vil_clamp_pixel(T const& b, double range_min, double range_max))

VIL_CLAMP_PIXEL_INSTANTIATE(unsigned char);
VIL_CLAMP_PIXEL_INSTANTIATE(unsigned short);
VIL_CLAMP_PIXEL_INSTANTIATE(unsigned int);
VIL_CLAMP_PIXEL_INSTANTIATE(float);
VIL_CLAMP_PIXEL_INSTANTIATE(double);
