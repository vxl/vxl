// This is core/vil1/vil1_scale_intensities.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// vil1_scale_intensities
// Author: David Capel, Oxford RRG
// Created: 26 Jul 2000
//
//-----------------------------------------------------------------------------

#include "vil1_scale_intensities.h"
#include <vil1/vil1_scale_intensities_image_impl.h>

vil1_image vil1_scale_intensities(vil1_image src, double scale, double shift) {
  return new vil1_scale_intensities_image_impl(src, scale, shift);
}
