#ifndef vcl_cmath_h_
#define vcl_cmath_h_

#ifdef __GNUG__
#pragma interface "vcl_cmath.h"
#endif

// [26.5.6] In addition to the signatures from the C header
// <math.h>, the C++ header <cmath> adds certain overloaded
// forms of the usual double-precision functions.
// The following signatures should be available for float, 
// double and long double :
//  T abs(T );
//  T acos(T );
//  T asin(T );
//  T atan(T );
//  T atan2(T, T );
//  T ceil(T );
//  T cos(T );
//  T cosh(T );
//  T exp(T );
//  T fabs(T );
//  T floor(T );
//  T fmod(T, T);
//  T frexp(T, int *);
//  T ldexp(float, int);
//  T log(T );
//  T log10(T );
//  T modf(T, T *);
//  T pow(T, T );
//  T pow(T, int );
//  T sin(T );
//  T sinh(T );
//  T sqrt(T );
//  T tan(T );
//  T tanh(T );

#include <vcl/vcl_compiler.h>

// 1. include system header
#if defined(VCL_GCC_27) || defined(VCL_SGI_CC_720)
# include <math.h>
#else // iso
# include <vcl/iso/vcl_cmath.h>
#endif


// 1.5 fix system header.
#if defined(VCL_SUNPRO_CC_50)
# include <math.h> // for the HUGE_VAL macro.
# undef M_PI_4     // avoid redef. it's non-iso anyway.
#endif
#if defined (linux) && defined (__OPTIMIZE__)
// * avoid infinite recursion when calling vnl_math::isfinite().
// * avoid symbol in object file being called vnl_math::_isinf.
# undef isinf  
// * avoid that vnl_math::isnan is redefined in <math.h>.
# undef isnan  
#endif


// 2. define vcl_abs() inline functions.
#ifndef vcl_abs
# if defined(VCL_EGCS) || defined(VCL_GCC_295)
#  define vcl_abs abs

# elif defined(VCL_GCC_27) || defined(VCL_SGI_CC_720)
inline float       vcl_abs (float  x) { return (x >= 0.0f) ? x : -x; }
inline double      vcl_abs (double x) { return fabs (x); }
inline long double vcl_abs (long double x) { return fabs (x); }
# else
#  define vcl_abs std::abs
# endif
#endif


// vcl_min/vcl_max do not belong in this file. They should be
// in vcl_algorithm.h, even for win32.
#ifdef WIN32
template <class T>
inline T vcl_max(T const& a, T const& b)
{
  return (a > b) ? a : b;
}

template <class T>
inline T vcl_min(T const& a, T const& b)
{
  return (a < b) ? a : b;
}
#endif


// 3. extensions.
#if defined(WIN32) || defined(VCL_SUNPRO_CC_50)
#define VCL_NO_DRAND48
#endif

#endif
