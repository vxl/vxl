#ifndef vepl_moment_h_
#define vepl_moment_h_

// .NAME vepl_moment - computation of n-th order moment
// .LIBRARY vepl
// .HEADER vxl package
// .INCLUDE vepl/vepl_moment.h
// .FILE vepl_moment.txx
//
// .SECTION Description
//   The n-th order moment operation replaces a pixel with the expected value
//   of x^n of its surrounding pixels, in a certain neighbourhood. Here the
//   neighbourhood is a arbitrary rectangular mask, the height and width of which
//   are passed to the constructor.
//
//   A fast computation method is used which needs only seven + or - operations per
//   pixel (except for initialization of first row and column) because it uses
//   computed values of previous pixels.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven (ESAT/PSI), 28 April 2001

#include <vil/vil_image.h>

vil_image vepl_moment(vil_image const& , int order, int width=3, int height=3);

#endif // vepl_moment_h_
