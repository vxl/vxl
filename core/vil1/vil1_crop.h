//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_crop_h_
#define vil_crop_h_
// Author: awf@robots.ox.ac.uk
// Created: 16 Feb 00

#include <vil/vil_fwd.h>

//: Copy SRC to DST.
// Images must be exactly the same dimensions, but datatype isn't matched
vil_image vil_crop(vil_image SRC, int x0, int y0, int w, int h);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_crop.

