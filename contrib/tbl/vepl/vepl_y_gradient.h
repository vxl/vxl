#ifndef vepl_y_gradient_h_
#define vepl_y_gradient_h_

// .NAME vepl_y_gradient - Convolve image with vertical [-1 1] filter
// .LIBRARY vepl
// .HEADER vxl package
// .INCLUDE vepl/vepl_y_gradient.h
// .FILE vepl_y_gradient.txx
//
// .SECTION Description
//   Note that the user has to make sure that the output values fall into
//   the output pixel range, by specifying appropriate scale and shift;
//   wrap-around will occur if it does not.
//
//   Note also the implicit use of DataOut::DataOut(DataIn),
//   which you probably will have to provide when DataIn and DataOut
//   are not the same type.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven (ESAT/PSI), 28 April 2001

#include <vil/vil_image.h>

vil_image vepl_y_gradient(vil_image const& , double scale=1.0, double shift=0.0);

#endif // vepl_y_gradient_h_
