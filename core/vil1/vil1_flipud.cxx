/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_flipud.h"
#include <vil/vil_flipud_impl.h>

vil_image vil_flipud(vil_image const &I) {
  return new vil_flipud_impl(I);
}
