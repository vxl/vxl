#ifndef vepl_dyadic_h_
#define vepl_dyadic_h_

//:
// \file
// \brief apply a (fixed) function to all (out,in) pixel pairs
//
//   Typical examples are adding or subtracting images pixel-wise.
//
//   Note that the output image (first argument) has to be initialized
//   in advance, as its pixel values are actually both read and written.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   28 April 2001

#include <vil/vil_image.h>

//: add the second image to the first one pixelwise
void vepl_dyadic_sum(vil_image , vil_image const& );
//: subtract the second image from the first one pixelwise
void vepl_dyadic_dif(vil_image , vil_image const& );
//: replace pixels of the first image by minimum pixel value of both
void vepl_dyadic_min(vil_image , vil_image const& );
//: replace pixels of the first image by maximum pixel value of both
void vepl_dyadic_max(vil_image , vil_image const& );

#endif // vepl_dyadic_h_
