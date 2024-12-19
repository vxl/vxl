// This is core/vil1/vil1_smooth.h
#ifndef vil1_smooth_h_
#define vil1_smooth_h_
//:
// \file
// \brief Functions to convolve a vil1_image with a smoothing kernel
// \author fsm

#include "vil1_image.h"

//: Convolve a vil1_image with a separable symmetric kernel.
// The 1-D half-kernel is supplied.
vil1_image
vil1_smooth_byte_separable_symmetric(const float kernel[], unsigned N, const vil1_image &);

//: Gaussian smooth an image.
vil1_image
vil1_smooth_gaussian(const vil1_image &, double sigma);

#endif // vil1_smooth_h_
