/*
  fsm@robots.ox.ac.uk
*/
#include "f2c.h"
#include <math.h>

doublereal cdabs_(doublecomplex const *z) {
  return hypot(z->r, z->i); /* is this what cdabs_() is supposed to do? */
}
