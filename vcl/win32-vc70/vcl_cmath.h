#ifndef vcl_win32_cmath_h_
#define vcl_win32_cmath_h_

#include <cmath>

namespace std {
  inline long double abs(long double  x) { return x >= 0 ? x : -x; } 
  inline double abs(double  x) { return x >= 0 ? x : -x; }
  inline float  abs(float x) { return x >= 0 ? x : -x; }
  inline float  sqrt(float f) { return float(std::sqrt(double(f))); }
  inline long double  sqrt(long double f) { return (long double)(::sqrt(double(f))); }
}

#define vcl_generic_cmath_STD std

#include "../generic/vcl_cmath.h"

#endif // vcl_cmath_h_

