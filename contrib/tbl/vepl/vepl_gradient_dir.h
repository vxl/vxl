#ifndef vepl_gradient_dir_h_
#define vepl_gradient_dir_h_

// .NAME vepl_gradient_dir - atan2 of vepl_x_gradient and vepl_y_gradient
// .LIBRARY vepl
// .HEADER vxl package
// .INCLUDE vepl/vepl_gradient_dir.h
// .FILE vepl_gradient_dir.txx
//
// .SECTION Description
//   Note that the pixel types must support conversion from/to double
//   where the atan2 computation takes place.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven (ESAT/PSI), 28 April 2001

#include <vil/vil_image.h>

vil_image vepl_gradient_dir(vil_image const& , double scale=1.0, double shift=0.0);

#endif // vepl_gradient_dir_h_
