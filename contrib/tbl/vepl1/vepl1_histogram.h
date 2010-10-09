#ifndef vepl1_histogram_h_
#define vepl1_histogram_h_
//:
// \file
// \brief pixel value histogram of 2D image
//
//   The vepl1_histogram class creates a pixel value histogram of a 2D image
//   and writes it into a vcl_vector<unsigned int>.
//
//   The number of bins for ubyte images is 256, for ushort images 65536,
//   and for float or double image it is 1000.
//
//   Note that, as opposed to vipl_histogram, the output bins are initialised
//   to 0, as one expects.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   28 April 2001
//
// \verbatim
//  Modifications
//   25-May-2002 - Peter Vanroose - changed return type from vil1_image to vcl_vector<uint>
// \endverbatim

#include <vil1/vil1_image.h>
#include <vcl_vector.h>

//: pixel value histogram of 2D image, stored in a 256x1 output image
vcl_vector<unsigned int> vepl1_histogram(vil1_image const& );

#endif // vepl1_histogram_h_
