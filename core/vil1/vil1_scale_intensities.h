// This is core/vil1/vil1_scale_intensities.h
#ifndef vil1_scale_intensities_h_
#define vil1_scale_intensities_h_
//:
// \file
// \brief Templated functions to scale/shift image pixel values
// \author    David Capel

#include <vil1/vil1_image.h>

//: Adaptor which returns a vil1_image with pixel components scaled/shifted
// I.e. performs a contrast/brightness operation.
//
//  (I_new = scale * I_old  + shift)

vil1_image vil1_scale_intensities(const vil1_image& src, double scale, double shift);

#endif // vil1_scale_intensities_h_
