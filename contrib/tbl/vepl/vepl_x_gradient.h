#ifndef vepl_x_gradient_h_
#define vepl_x_gradient_h_

//:
// \file
// \brief Convolve image with horizontal [-1 1] filter
//
//   Note that the user has to make sure that the output values fall into
//   the output pixel range, by specifying appropriate scale and shift;
//   wrap-around will occur if it does not.
//
//   Note also the implicit use of DataOut::DataOut(DataIn),
//   which you probably will have to provide when DataIn and DataOut
//   are not the same type.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   28 April 2001

#include <vil/vil_image.h>

//: Convolve image with horizontal [-1 1] filter
vil_image vepl_x_gradient(vil_image const& , double scale=1.0, double shift=0.0);

#endif // vepl_x_gradient_h_
