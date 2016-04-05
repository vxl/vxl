#ifndef vepl_convert_h_
#define vepl_convert_h_
//:
// \file
// \brief convert between arbitrary image types
//
//   This class actually copies one image to an other, pixel-wise.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002
//
// \verbatim
//  Modifications
//   Peter Vanroose - 20 aug 2003 - changed parameter and return types from vil_image_view_base_sptr to vil_image_resource_sptr
// \endverbatim

#include <vil/vil_image_resource.h>

//: convert between arbitrary image types
template <class D>
vil_image_resource_sptr vepl_convert(vil_image_resource_sptr, D dummy);

#ifdef INSTANTIATE_TEMPLATES
#include "vepl_convert.hxx"
#endif

#endif // vepl_convert_h_
