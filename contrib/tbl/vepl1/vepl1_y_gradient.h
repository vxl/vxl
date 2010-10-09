#ifndef vepl1_y_gradient_h_
#define vepl1_y_gradient_h_
//:
// \file
// \brief Convolve image with vertical [-1 1] filter
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

#include <vil1/vil1_image.h>

//: Convolve image with vertical [-1 1] filter
vil1_image vepl1_y_gradient(vil1_image const& , double scale=1.0, double shift=0.0);

#endif // vepl1_y_gradient_h_
