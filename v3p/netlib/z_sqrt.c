/*
  fsm@robots.ox.ac.uk
*/
#include "f2c.h"
#undef abs
#include <math.h>

/* This is here until I find the fortran code for z_sqrt(). */

void z_sqrt(doublecomplex *dst, doublecomplex *src)
{
  double a=src->r;
  double b=src->i;

  double theta = atan2(b,a);
  double r = hypot(a,b);

  theta *= 0.5;
  r = sqrt(r);

  dst->r = r * cos(theta);
  dst->i = r * sin(theta);
}

