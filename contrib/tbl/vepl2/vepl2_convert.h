#ifndef vepl2_convert_h_
#define vepl2_convert_h_

//:
// \file
// \brief convert between arbitrary image types
//
//   This class actually copies one image to an other, pixel-wise.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002

#include <vil2/vil2_image_view_base.h>

//: convert between arbitrary image types
template <class D>
vil2_image_view_base_sptr vepl2_convert(vil2_image_view_base const& , D dummy);

#ifdef INSTANTIATE_TEMPLATES
#include "vepl2_convert.txx"
#endif

#endif // vepl2_convert_h_
