#ifndef vepl_gaussian_convolution_h_
#define vepl_gaussian_convolution_h_

// .NAME vepl_gaussian_convolution - gaussian smoothing
// .LIBRARY vepl
// .HEADER vxl package
// .INCLUDE vepl/vepl_gaussian_convolution.h
// .FILE vepl_gaussian_convolution.txx
//
// .SECTION Description
//   Gaussian filtering is an operation that replaces a pixel with the
//   average value of its surrounding pixels, in a certain neighbourhood,
//   according to a Gaussian distribution (with given sigma= std deviation).
//   (The window is cut when `cutoff' (default: 0.01) of the probability mass
//   lies out of the window.)
//
//   Note that DataIn values must allow addition and multiplication with floats;
//   and that the result be expressible as DataOut, possibly after rounding.
//   Probably only float and double make sense as DataOut (?)
//
//   As this is a separable filter, it is implemented as a row-based 1-D filter
//   followed by a column-based 1-D step.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven (ESAT/PSI), 28 April 2001

#include <vil/vil_image.h>

vil_image vepl_gaussian_convolution(vil_image const& , double sigma=1, double cutoff=0.01);

#endif // vepl_gaussian_convolution_h_
