#ifndef OTAGO_INLINES_H__
#define OTAGO_INLINES_H__

#include <vcl_cmath.h>

inline bool epsilon_equals(double d1, double d2, double epsilon=1.0e-6)
{
  return  vcl_fabs(d1-d2)<epsilon;
}

#endif // OTAGO_INLINES_H__
