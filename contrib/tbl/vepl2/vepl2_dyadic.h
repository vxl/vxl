#ifndef vepl2_dyadic_h_
#define vepl2_dyadic_h_
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
// \date   7 October 2002

#include <vil2/vil2_image_view_base.h>

//: add the second image to the first one pixelwise
void vepl2_dyadic_sum(vil2_image_view_base_sptr , vil2_image_view_base const& );
//: subtract the second image from the first one pixelwise
void vepl2_dyadic_dif(vil2_image_view_base_sptr , vil2_image_view_base const& );
//: replace pixels of the first image by minimum pixel value of both
void vepl2_dyadic_min(vil2_image_view_base_sptr , vil2_image_view_base const& );
//: replace pixels of the first image by maximum pixel value of both
void vepl2_dyadic_max(vil2_image_view_base_sptr , vil2_image_view_base const& );

#endif // vepl2_dyadic_h_
