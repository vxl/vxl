#ifndef vil_scale_intensities_h_
#define vil_scale_intensities_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vil/vil_scale_intensities.h

//:
// \file
// \brief Templated functions to scale/shift image pixel values
// \author    David Capel

#include <vil/vil_image.h>

//: Adaptor which returns a vil_image with pixel components scaled/shifted
// i.e. performs a contrast/brightness operation.
//  (I_new = scale * I_old  + shift)
vil_image vil_scale_intensities(vil_image src, double scale, double shift);

#endif // vil_scale_intensities_h_
