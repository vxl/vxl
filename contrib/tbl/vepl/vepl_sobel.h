#ifndef vepl_sobel_h_
#define vepl_sobel_h_
//:
// \file
// \brief Sobel convolution filter
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002
//
// \verbatim
//  Modifications
//   Peter Vanroose - 20 aug 2003 - changed parameter and return types from vil_image_view_base_sptr to vil_image_resource_sptr
// \endverbatim

#include <vil/vil_image_resource.h>

//: Sobel convolution filter
vil_image_resource_sptr vepl_sobel(const vil_image_resource_sptr& );

#endif // vepl_sobel_h_
