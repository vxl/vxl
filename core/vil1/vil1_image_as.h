#ifndef vil_image_as_h_
#define vil_image_as_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_image_as
// .INCLUDE vil/vil_image_as.h
// .FILE vil_image_as.cxx
// @author fsm@robots.ox.ac.uk

#include <vil/vil_image.h>

vil_image vil_image_as_int(vil_image const &);
vil_image vil_image_as_byte(vil_image const &);
vil_image vil_image_as_float(vil_image const &);
vil_image vil_image_as_double(vil_image const &);

vil_image vil_image_as_rgb_byte(vil_image const &);
vil_image vil_image_as_rgb_float(vil_image const &);

#endif
