//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_crop_h_
#define vil_crop_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_crop
// .INCLUDE vil/vil_crop.h
// .FILE vil_crop.cxx
// .SECTION Author
//    awf@robots.ox.ac.uk
// Created: 16 Feb 00
//
// .SECTION Modifications
//     000216 AWF Initial version.

#include <vil/vil_fwd.h>

//: Crop to a region of SRC.
vil_image vil_crop(vil_image SRC, int x0, int y0, int w, int h);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_crop.

