#ifndef vepl2_gaussian_convolution_h_
#define vepl2_gaussian_convolution_h_

//:
// \file
// \brief gaussian smoothing
//
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
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002

#include <vil2/vil2_image_view_base.h>

//: gaussian smoothing with given sigma (default 1)
vil2_image_view_base_sptr vepl2_gaussian_convolution(vil2_image_view_base const& , double sigma=1, double cutoff=0.01);

#endif // vepl2_gaussian_convolution_h_
