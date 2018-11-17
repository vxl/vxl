#ifndef vepl_histogram_h_
#define vepl_histogram_h_
//:
// \file
// \brief pixel value histogram of 2D image
//
//   The vepl_histogram class creates a pixel value histogram of a 2D image
//   and writes it into a std::vector<unsigned int>.
//
//   The number of bins for ubyte images is 256, for ushort images 65536,
//   and for float or double image it is 1000.
//
//   Note that, as opposed to vipl_histogram, the output bins are initialised
//   to 0, as one expects.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002
//
// \verbatim
//  Modifications
//   Peter Vanroose - 20 aug 2003 - changed parameter from vil_image_view_base_sptr to vil_image_resource_sptr
// \endverbatim

#include <iostream>
#include <vector>
#include <vil/vil_image_resource.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: pixel value histogram of 2D image, stored in a vector
std::vector<unsigned int> vepl_histogram(const vil_image_resource_sptr& );

#endif // vepl_histogram_h_
