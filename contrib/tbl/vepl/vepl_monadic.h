#ifndef vepl_monadic_h_
#define vepl_monadic_h_
//:
// \file
// \brief apply any (fixed) function to all pixels
//
//   The only parameter to be passed to the constructor must be a (monadic)
//   function that takes a pixel value from the input image and produces
//   a pixel value of the output image, i.e., its signature must be
//   DataOut f(DataIn const&).
//   A typical example is, e.g., log(), but any point operator (like e.g.
//   thresholding) could be implemented through this more general monadic IP
//   operator, notably shift or scale of intensity values.
//
//   Note that the input and output images are allowed to be identical.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002
//
// \verbatim
//  Modifications
//   Peter Vanroose - 20 aug 2003 - changed parameter and return types from vil_image_view_base_sptr to vil_image_resource_sptr
// \endverbatim

#include <vil/vil_image_resource.h>

//: replace pixels by their absolute value
vil_image_resource_sptr vepl_monadic_abs(const vil_image_resource_sptr& );
//: replace pixels by their square root
vil_image_resource_sptr vepl_monadic_sqrt(const vil_image_resource_sptr& );
//: replace pixels by their square
vil_image_resource_sptr vepl_monadic_sqr(const vil_image_resource_sptr& );
//: replace pixels by a linear distortion
vil_image_resource_sptr vepl_monadic_shear(const vil_image_resource_sptr& , double shift, double scale);

#endif // vepl_monadic_h_
