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
// \date   28 April 2001

#include <vil/vil_image.h>

//: replace pixels by their absolute value
vil_image vepl_monadic_abs(vil_image const& );
//: replace pixels by their square root
vil_image vepl_monadic_sqrt(vil_image const& );
//: replace pixels by their square
vil_image vepl_monadic_sqr(vil_image const& );
//: replace pixels by a linear distortion
vil_image vepl_monadic_shear(vil_image const& , double shift, double scale);

#endif // vepl_monadic_h_
