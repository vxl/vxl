// This is core/vil1/vil1_copy.h
#ifndef vil1_copy_h_
#define vil1_copy_h_
//:
// \file
// \brief Image copy function
// \author awf@robots.ox.ac.uk
// \date 16 Feb 00
//
// \verbatim
// Modifications
//     000216 AWF Initial version.
//     000217 JS  components*planes because get_section returns RGBRGB
// \endverbatim

#include <vil1/vil1_fwd.h>

//: Copy SRC to DST.
// Images must be exactly the same dimensions, but datatype isn't matched
void vil1_copy(vil1_image const& src, vil1_image& DST);

//: Return a memory image copy of SRC.
vil1_memory_image vil1_copy(vil1_image const& src);

#endif // vil1_copy_h_
