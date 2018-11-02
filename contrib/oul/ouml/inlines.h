#ifndef OTAGO_INLINES_H__
#define OTAGO_INLINES_H__

#include <iostream>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

inline bool epsilon_equals(double d1, double d2, double epsilon=1.0e-6)
{
  return  std::fabs(d1-d2)<epsilon;
}

#endif // OTAGO_INLINES_H__
