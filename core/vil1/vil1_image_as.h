#ifndef vil_image_as_h_
#define vil_image_as_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vil/vil_image.h>

vil_image vil_image_as_rgb(vil_image const &); // "rgb_byte" would be more appropriate
vil_image vil_image_as_byte(vil_image const &);
vil_image vil_image_as_float(vil_image const &);

#endif
