//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vil_crop.h"
#endif
//
// Class: vil_crop
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 16 Feb 00
// Modifications:
//   000216 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "vil_crop.h"

#include <vil/vil_crop_image.h>

vil_generic_image* vil_crop(vil_generic_image* SRC, int x0, int y0, int w, int h)
{
  return new vil_crop_image(SRC, x0, y0, w, h);
}
