#ifndef vepl_gradient_mag_h_
#define vepl_gradient_mag_h_
//:
// \file
// \brief gradient magnitude: sqrt of x_gradient square plus y_gradient square
//
//   Note that the pixel types must support conversion from/to double
//   where the sqrt computation takes place.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002
//
// \verbatim
//  Modifications
//   Peter Vanroose - 20 aug 2003 - changed parameter and return types from vil_image_view_base_sptr to vil_image_resource_sptr
// \endverbatim

#include <vil/vil_image_resource.h>

//: gradient magnitude: sqrt of x_gradient square plus y_gradient square
vil_image_resource_sptr vepl_gradient_mag(const vil_image_resource_sptr& , double scale=1.0, double shift=0.0);

#endif // vepl_gradient_mag_h_
