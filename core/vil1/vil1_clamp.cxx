#ifdef __GNUC__
#pragma implementation "vil_clamp.h"
#endif
//
// Class: vil_clamp
// Author: David Capel, Oxford RRG
// Created: 14 Aug 98
// Modifications:
//
//-----------------------------------------------------------------------------

#include "vil_clamp.h"
#include <vil/vil_clamp_image_impl.h>

vil_image vil_clamp(vil_image src, double range_min, double range_max) {
  return new vil_clamp_image_impl(src, range_min, range_max);
}
