#ifndef vepl2_gradient_dir_h_
#define vepl2_gradient_dir_h_

//:
// \file
// \brief gradient direction: atan2 of x_gradient and y_gradient
//
//   Note that the pixel types must support conversion from/to double
//   where the atan2 computation takes place.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002

#include <vil2/vil2_image_view_base.h>

//: gradient direction: atan2 of x_gradient and y_gradient
vil2_image_view_base_sptr vepl2_gradient_dir(vil2_image_view_base const& , double scale=1.0, double shift=0.0);

#endif // vepl2_gradient_dir_h_
