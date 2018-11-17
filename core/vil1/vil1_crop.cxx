// This is core/vil1/vil1_crop.cxx
//
// vil1_crop
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 16 Feb 00
//
//-----------------------------------------------------------------------------

#include "vil1_crop.h"

#include <vil1/vil1_crop_image_impl.h>
#include <vil1/vil1_image.h>

vil1_image vil1_crop(const vil1_image& SRC, int x0, int y0, int w, int h)
{
  return new vil1_crop_image_impl(SRC, x0, y0, w, h);
}
