#ifndef vepl1_dilate_disk_h_
#define vepl1_dilate_disk_h_
//:
// \file
// \brief morphological dilation with circular element
//
//  Dilation is a morphological operation that replaces a pixel with the
//  maximum value of its surrounding pixels, in a certain neighbourhood.
//  Here, the neighbourhood is circular, with an arbitrary (float) radius,
//  which is to be passed to the constructor.
//
//  Note that the function max(DataIn,DataIn) is being used; for
//  non-scalar data types (like colour pixels) an appropriate max() function
//  must thus be supplied.
//
//  Note also the implicit use of DataOut::DataOut(DataIn),
//  which you probably will have to provide when DataIn and DataOut
//  are not the same type.  It could even be argued that these types should
//  always be the same!
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   28 April 2001
//
// \verbatim
//  Modifications
//   12/97 updated by Tboult to use new codegen form and have valid (public
//          agreed) ctor and to use preop and postop to define/destroy the mask.
// \endverbatim

#include <vil1/vil1_image.h>

//: morphological dilation with circular element
vil1_image vepl1_dilate_disk(vil1_image const& , float radius);

#endif // vepl1_dilate_disk_h_
