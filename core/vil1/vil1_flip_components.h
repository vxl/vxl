// This is vxl/vil/vil_flip_components.h
#ifndef vil_flip_components_h_
#define vil_flip_components_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief swap the "R" and "B" colour components of an image
// \author Peter.Vanroose@esat.kuleuven.ac.be

#include <vil/vil_image.h>

//: swap the "R" and "B" colour components of an image
vil_image vil_flip_components(vil_image const &);

#endif // vil_flip_components_h_
