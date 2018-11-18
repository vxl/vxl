// This is core/vidl/vidl_v4l2_pixel_format.h
#ifndef vidl_v4l2_pixel_format_h_
#define vidl_v4l2_pixel_format_h_
//:
// \file
// \brief Correspondences between vidl pixel format and v4l2 pixel format
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//     4 Jul 2008 Created (A. Garrido)
//\endverbatim


#include "vidl_pixel_format.h"

//: convert from vidl pixel format to v4l2 pixel format
unsigned int vidl_to_v4l2 (vidl_pixel_format pf);
//: convert from v4l2 pixel format to vidl pixel format
vidl_pixel_format v4l2_to_vidl(unsigned int pf);

#endif // vidl_v4l2_pixel_format_h_
