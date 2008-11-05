// This is brl/bbas/vidl2/v4l2_pixel_format.h
#ifndef v4l2_pixel_format_h_
#define v4l2_pixel_format_h_
//:
// \file
// \brief Correspondences between vidl2 pixel format and v4l2 pixel format
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//     4 Jul 2009 Created (A. Garrido)
//\endverbatim


#include "vidl2_pixel_format.h"

//: convert from vidl2 pixel format to v4l2 pixel format
int vidl2_to_v4l2 (vidl2_pixel_format pf);
//: convert from v4l2 pixel format to vidl2 pixel format
vidl2_pixel_format v4l2_to_vidl2(int pf);

#endif // v4l2_pixel_format_h_

