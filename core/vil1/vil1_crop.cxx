//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// Class: vil_crop
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 16 Feb 00
//
//-----------------------------------------------------------------------------

#include "vil_crop.h"

#include <vil/vil_crop_image_impl.h>
#include <vil/vil_image.h>

vil_image vil_crop(vil_image SRC, int x0, int y0, int w, int h)
{
  return new vil_crop_image_impl(SRC, x0, y0, w, h);
}
