#ifndef vil_skip_h_
#define vil_skip_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_skip - a function
// .INCLUDE vil/vil_skip.h
// .FILE vil_skip.cxx
// \author fsm@robots.ox.ac.uk

#include <vil/vil_image.h>

vil_image vil_skip(vil_image const &, unsigned, unsigned);

#endif
