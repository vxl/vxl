/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vcl_cmath.h"
#endif
#include "vcl_cmath.h"

#ifdef __GNUC__
double tickle_cmath_inlines()
{
  float f = vcl_abs(1.0f);
  double d = vcl_abs(1.0);
  return f + d;
}
#endif
