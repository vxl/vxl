#ifndef vepl2_histogram_h_
#define vepl2_histogram_h_

//:
// \file
// \brief pixel value histogram of 2D image
//
//   The vepl2_histogram class creates a pixel value histogram of a 2D image
//   and writes it into a vcl_vector<unsigned int>.
//
//   The number of bins for ubyte images is 256, for ushort images 65536,
//   and for float or double image it is 1000.
//
//   Note that, as opposed to vipl_histogram, the output bins are initialised
//   to 0, as one expects.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002

#include <vil2/vil2_image_view_base.h>
#include <vcl_vector.h>

//: pixel value histogram of 2D image, stored in a 256x1 output image
vcl_vector<unsigned int> vepl2_histogram(vil2_image_view_base const& );

#endif // vepl2_histogram_h_
