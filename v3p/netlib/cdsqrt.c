/*
  fsm@robots.ox.ac.uk
*/
#include "f2c.h"
#include <math.h>

doublereal cdsqrt_(doublecomplex const *z) {
  return hypot(z->r, z->i);
}
