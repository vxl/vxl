// This is core/vil1/vil1_flip_components.h
#ifndef vil1_flip_components_h_
#define vil1_flip_components_h_
//:
// \file
// \brief swap the "R" and "B" colour components of an image
// \author Peter Vanroose, ESAT, KULeuven.

#include "vil1_image.h"

//: swap the "R" and "B" colour components of an image
vil1_image
vil1_flip_components(const vil1_image &);

#endif // vil1_flip_components_h_
