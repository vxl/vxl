#ifndef vepl2_sobel_h_
#define vepl2_sobel_h_

//:
// \file
// \brief sobel convolution filter
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002

#include <vil2/vil2_image_view_base.h>

//: sobel convolution filter
vil2_image_view_base_sptr vepl2_sobel(vil2_image_view_base const& );

#endif // vepl2_sobel_h_
