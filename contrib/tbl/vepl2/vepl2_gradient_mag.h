#ifndef vepl2_gradient_mag_h_
#define vepl2_gradient_mag_h_

//:
// \file
// \brief gradient magnitude: sqrt of x_gradient square plus y_gradient square
//
//   Note that the pixel types must support conversion from/to double
//   where the sqrt computation takes place.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002

#include <vil2/vil2_image_view_base.h>

//: gradient magnitude: sqrt of x_gradient square plus y_gradient square
vil2_image_view_base_sptr vepl2_gradient_mag(vil2_image_view_base const& , double scale=1.0, double shift=0.0);

#endif // vepl2_gradient_mag_h_
