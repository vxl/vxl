#ifndef vepl1_gradient_mag_h_
#define vepl1_gradient_mag_h_
//:
// \file
// \brief gradient magnitude: sqrt of x_gradient square plus y_gradient square
//
//   Note that the pixel types must support conversion from/to double
//   where the sqrt computation takes place.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   28 April 2001

#include <vil1/vil1_image.h>

//: gradient magnitude: sqrt of x_gradient square plus y_gradient square
vil1_image vepl1_gradient_mag(vil1_image const& , double scale=1.0, double shift=0.0);

#endif // vepl1_gradient_mag_h_
