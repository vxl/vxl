#ifndef vil_flipud_h_
#define vil_flipud_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vil/vil_flipud.h

//:
// \file
// \brief Turn an image upside-down.
// \author fsm@robots.ox.ac.uk

#include <vil/vil_image.h>

//: Turn an image upside-down
vil_image vil_flipud(vil_image const &);

#endif // vil_flipud_h_
