// This is vxl/vil/vil_smooth.h
#ifndef vil_smooth_h_
#define vil_smooth_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Functions to convolve a vil_image with a smoothing kernel
// \author fsm@robots.ox.ac.uk

#include <vil/vil_image.h>

//: Convolve a vil_image with a separable symmetric kernel.
// The 1-D half-kernel is supplied.
vil_image vil_smooth_byte_separable_symmetric(float const kernel[], unsigned N, vil_image const &);

//: Gaussian smooth an image.
vil_image vil_smooth_gaussian(vil_image const &, double sigma);

#endif // vil_smooth_h_
