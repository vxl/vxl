#ifndef vepl_dyadic_h_
#define vepl_dyadic_h_

// .NAME vepl_dyadic - apply any (fixed) function to all (out,in) pixel pairs
// .LIBRARY vepl
// .HEADER vxl package
// .INCLUDE vepl/vepl_dyadic.h
// .FILE vepl_dyadic.txx
//
// .SECTION Description
//   The only parameter to be passed to the constructor must be a (dyadic)
//   function that takes two pixel values, one from the output image and
//   one from the input image, and changes its first argument is some way.
//   The function should not return a value, i.e., its signature must be
//   void f(DataOut&, DataIn const&).
//   Typical examples are adding or subtracting images pixel-wise.
//
//   Note that the output image has to be initialized in advance, as its
//   pixel values are actually both read and written.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven (ESAT/PSI), 28 April 2001

#include <vil/vil_image.h>

void vepl_dyadic_sum(vil_image , vil_image const& );
void vepl_dyadic_dif(vil_image , vil_image const& );
void vepl_dyadic_min(vil_image , vil_image const& );
void vepl_dyadic_max(vil_image , vil_image const& );

#endif // vepl_dyadic_h_
