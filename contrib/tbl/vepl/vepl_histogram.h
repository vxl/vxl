#ifndef vepl_histogram_h_
#define vepl_histogram_h_

//:
// \file
// \brief pixel value histogram of 2D image
//
//   The vipl_histogram class creates a pixel value histogram of a 2D image
//   and writes it into a "1D" image (actually a row of a 2D image).
//
//   The constructor takes three (defaulted) arguments:
//   the scaling SI to be applied to the input pixel values,
//   the translation (shift) SHI to be applied to the input pixel values,
//   and the scaling SO to be applied to the output histogram values.
//   By default, of course, SI=1, SHI=0 and SO=1.
//
//   By specifying SI, one actually specifies the bin size of the histogram:
//   e.g., SI=10 means that all pixel values in the range (e.g.) 15 to 25 (15
//   exclusive, 25 inclusive) contribute to the same histogram count (bin 2).
//   Clearly, for `int'-like pixels, the natural value for SI is 1, but
//   for float pixels, SI should be set to a reasonable bin size.
//
//   Combined with SI, one can set SHI to set the boundaries of those bins:
//   e.g., SI=10 and SHI=15 will map pixels values in the range 0 to 10 to
//   histogram bin 2.
//
//   The third parameter, SO, re-scales the value of the output bin count.
//   When set to 1 (the default), the pixel values of the output image are
//   the (integer) pixel counts of the bin which they represent. Thus
//   the `natural' output pixel type is int.  If this is not desired, SO
//   can be set to make sure that the histogram values fall within the
//   pixel value range of the output data type.
//
//   There are two other attributes, not specified in the constructor, which default to 0.
//   The first, indexout defines what row of the output image will be used
//   for the actual storage of the histogram values.  By making the output
//   image multi-row and changing this the same filter can do multiple
//   histograms.
//
//   The final attribute, checkrange is a boolean that determines if the histogram
//   should do range checking on bin access.  This can be useful for float
//   images. if a value is below the first image access or above the last,
//   its set to the extremal value.
//
//   Note it does not limit the maximum of bins so they may wrap around for unsigned
//   and accessing a small number of bins from a large dynamic range image may segfault
//   if check bounds is not turned on (off by default)
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   28 April 2001
//
// \verbatim
//  Modifications
//   25-May-2002 - Peter Vanroose - changed return type from vil_image to vcl_vector<uint>
// \endverbatim

#include <vil/vil_image.h>
#include <vcl_vector.h>

//: pixel value histogram of 2D image, stored in a 256x1 output image
vcl_vector<unsigned int> vepl_histogram(vil_image const& );

#endif // vepl_histogram_h_
