#ifndef vepl2_sobel_h_
#define vepl2_sobel_h_
//:
// \file
// \brief Sobel convolution filter
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002

#include <vil/vil_image_view_base.h>

//: Sobel convolution filter
vil_image_view_base_sptr vepl2_sobel(vil_image_view_base const& );

#endif // vepl2_sobel_h_
