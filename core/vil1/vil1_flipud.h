#ifndef vil_flipud_h_
#define vil_flipud_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_flipud
// .INCLUDE vil/vil_flipud
// .FILE vil_flipud.cxx
// .SECTION Author
//  fsm@robots.ox.ac.uk
//
// .SECTION Description
// purpose: turn an image upside-down.

#include <vil/vil_image.h>

vil_image vil_flipud(vil_image const &);

#endif // vil_flipud_h_
