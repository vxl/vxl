// This is core/vil1/vil1_clamp.cxx
//
// vil1_clamp
// Author: David Capel, Oxford RRG
// Created: 14 Aug 98
//
//-----------------------------------------------------------------------------

#include "vil1_clamp.h"
#include <vil1/vil1_clamp_image_impl.h>

vil1_image vil1_clamp(const vil1_image& src, double range_min, double range_max) {
  return new vil1_clamp_image_impl(src, range_min, range_max);
}

#undef VIL1_CLAMP_PIXEL_INSTANTIATE
#define VIL1_CLAMP_PIXEL_INSTANTIATE(T) \
/*template T vil1_clamp_pixel(T const& b, double range_min, double range_max) */

VIL1_CLAMP_PIXEL_INSTANTIATE(unsigned char);
VIL1_CLAMP_PIXEL_INSTANTIATE(unsigned short);
VIL1_CLAMP_PIXEL_INSTANTIATE(unsigned int);
VIL1_CLAMP_PIXEL_INSTANTIATE(float);
VIL1_CLAMP_PIXEL_INSTANTIATE(double);
