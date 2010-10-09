#ifndef vepl1_gradient_dir_h_
#define vepl1_gradient_dir_h_

//:
// \file
// \brief gradient direction: atan2 of x_gradient and y_gradient
//
//   Note that the pixel types must support conversion from/to double
//   where the atan2 computation takes place.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   28 April 2001

#include <vil1/vil1_image.h>

//: gradient direction: atan2 of x_gradient and y_gradient
vil1_image vepl1_gradient_dir(vil1_image const& , double scale=1.0, double shift=0.0);

#endif // vepl1_gradient_dir_h_
