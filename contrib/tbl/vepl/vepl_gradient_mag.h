#ifndef vepl_gradient_mag_h_
#define vepl_gradient_mag_h_

// .NAME vepl_gradient_mag - sqrt of vepl_x_gradient square plus vepl_y_gradient square
// .LIBRARY vepl
// .HEADER vxl package
// .INCLUDE vepl/vepl_gradient_mag.h
// .FILE vepl_gradient_mag.txx
//
// .SECTION Description
//   Note that the pixel types must support conversion from/to double
//   where the sqrt computation takes place.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven (ESAT/PSI), 28 April 2001

#include <vil/vil_image.h>

vil_image vepl_gradient_mag(vil_image const& , double scale=1.0, double shift=0.0);

#endif // vepl_gradient_mag_h_
