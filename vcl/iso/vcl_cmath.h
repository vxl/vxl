#ifndef vcl_iso_cmath_h_
#define vcl_iso_cmath_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <cmath>

// allow this for the time being.
using std::abs;
using std::acos;
using std::asin;
using std::atan2;
using std::atan;
using std::ceil;
using std::cos;
using std::cosh;
using std::exp;
using std::fabs;
using std::floor;
using std::fmod;
using std::frexp;
using std::ldexp;
using std::log10;
using std::log;
using std::modf;
using std::pow;
using std::sin;
using std::sinh;
using std::sqrt;
using std::tan;
using std::tanh;

#ifndef vcl_abs
#define vcl_abs   std::abs
#endif
#define vcl_acos  std::acos
#define vcl_asin  std::asin
#define vcl_atan2 std::atan2
#define vcl_atan  std::atan
#define vcl_ceil  std::ceil
#define vcl_cos   std::cos
#define vcl_cosh  std::cosh
#define vcl_exp   std::exp
#define vcl_fabs  std::fabs
#define vcl_floor std::floor
#define vcl_fmod  std::fmod
#define vcl_frexp std::frexp
#define vcl_ldexp std::ldexp
#define vcl_log10 std::log10
#define vcl_log   std::log
#define vcl_modf  std::modf
#define vcl_pow   std::pow
#define vcl_sin   std::sin
#define vcl_sinh  std::sinh
#define vcl_sqrt  std::sqrt
#define vcl_tan   std::tan
#define vcl_tanh  std::tanh

#endif
