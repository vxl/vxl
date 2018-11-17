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
// \date   7 October 2002
//
// \verbatim
//  Modifications
//   Peter Vanroose - 20 aug 2003 - changed parameter and return types from vil_image_view_base_sptr to vil_image_resource_sptr
// \endverbatim

#include <vil/vil_image_resource.h>

//: add the second image to the first one pixelwise
void vepl_dyadic_sum(const vil_image_resource_sptr&, const vil_image_resource_sptr&);
//: subtract the second image from the first one pixelwise
void vepl_dyadic_dif(const vil_image_resource_sptr&, const vil_image_resource_sptr&);
//: replace pixels of the first image by minimum pixel value of both
void vepl_dyadic_min(const vil_image_resource_sptr&, const vil_image_resource_sptr&);
//: replace pixels of the first image by maximum pixel value of both
void vepl_dyadic_max(const vil_image_resource_sptr&, const vil_image_resource_sptr&);

#endif // vepl_dyadic_h_
