#ifndef vepl2_monadic_h_
#define vepl2_monadic_h_

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

#include <vil2/vil2_image_view_base.h>

//: replace pixels by their absolute value
vil2_image_view_base_sptr vepl2_monadic_abs(vil2_image_view_base const& );
//: replace pixels by their square root
vil2_image_view_base_sptr vepl2_monadic_sqrt(vil2_image_view_base const& );
//: replace pixels by their square
vil2_image_view_base_sptr vepl2_monadic_sqr(vil2_image_view_base const& );
//: replace pixels by a linear distortion
vil2_image_view_base_sptr vepl2_monadic_shear(vil2_image_view_base const& , double shift, double scale);

#endif // vepl2_monadic_h_
