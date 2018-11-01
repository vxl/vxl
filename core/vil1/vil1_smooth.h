// This is core/vil1/vil1_smooth.h
#ifndef vil1_smooth_h_
#define vil1_smooth_h_
//:
// \file
// \brief Functions to convolve a vil1_image with a smoothing kernel
// \author fsm

#include <vil1/vil1_image.h>

//: Convolve a vil1_image with a separable symmetric kernel.
// The 1-D half-kernel is supplied.
vil1_image vil1_smooth_byte_separable_symmetric(float const kernel[], unsigned N, vil1_image const &);

//: Gaussian smooth an image.
vil1_image vil1_smooth_gaussian(vil1_image const &, double sigma);

#endif // vil1_smooth_h_
