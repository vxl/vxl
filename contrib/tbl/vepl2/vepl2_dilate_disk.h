#ifndef vepl2_dilate_disk_h_
#define vepl2_dilate_disk_h_

//:
// \file
// \brief morphological dilation with circular element
//
//   Dilation is a morphological operation that replaces a pixel with the
//   maximum value of its surrounding pixels, in a certain neighbourhood.
//   Here, the neighbourhood is circular, with an arbitrary (float) radius,
//   which is to be be passed to the constructor.
//
//   Note that the function max(DataIn,DataIn) is being used; for
//   non-scalar data types (like colour pixels) an appropriate max() function
//   must thus be supplied.
//
//   Note also the implicit use of DataOut::DataOut(DataIn),
//   which you probably will have to provide when DataIn and DataOut
//   are not the same type.  It could even be argued that these types should
//   always be the same!
//
//  updated by Tboult to use new codegen form and have valid (public argree) ctor
//  and to use preop and postop to define/destroy the mask. 12/97
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002

#include <vil2/vil2_image_view_base.h>

//: morphological dilation with circular element
vil2_image_view_base_sptr vepl2_dilate_disk(vil2_image_view_base const& , float radius);

#endif // vepl2_dilate_disk_h_
