// This is vxl/vil/vil_skip.cxx

/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_skip.h"
#include <vil/vil_skip_image_impl.h>

vil_image vil_skip(vil_image const &i, unsigned sx, unsigned sy) {
  return new vil_skip_image_impl(i, sx, sy);
}
