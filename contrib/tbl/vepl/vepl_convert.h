#ifndef vepl_convert_h_
#define vepl_convert_h_

// .NAME vepl_convert - convert between arbitrary image types
// .LIBRARY vepl
// .HEADER vxl package
// .INCLUDE vepl/vepl_convert.h
// .FILE vepl_convert.txx
//
// .SECTION Description
//   This class actually copies one image to an other, pixel-wise.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven (ESAT/PSI), 28 April 2001

#include <vil/vil_image.h>

template <class D>
vil_image vepl_convert(vil_image const& , D dummy);

#ifdef INSTANTIATE_TEMPLATES
#include "vepl_convert.txx"
#endif

#endif // vepl_convert_h_
