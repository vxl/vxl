/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
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


#if defined(__SUNPRO_CC) && (__SUNPRO_CC == 0x500)
// these symbols go missing at link time. I suspect
// they are not provided in the run-time library.
// fsm
namespace std {
  float atan2(float y, float x) { return float(::atan2(double(y), double(x))); }
  float sqrt(float x) { return float(::sqrt(double(x))); }
  double abs(double x) { return ::fabs(x); }
  float ceil(float x) { return float(::ceil(double(x))); }
  float floor(float x) { return float(::floor(double(x))); }
  float sin(float x) { return float(::sin(double(x))); }
  float fabs(float x) { return float(::fabs(double(x))); }
  float cos(float x) { return float(::cos(double(x))); }
  float pow(float x, float a) { return float(::pow(double(x), double(a))); }
  float exp(float x) { return float(::exp(double(x))); }
  float log(float x) { return float(::log(double(x))); }
  float acos(float x) { return float(::acos(double(x))); }
}
#endif
