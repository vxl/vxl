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

// Include system math.h and define some vcl_* inline functions.
// Some compilers may actually have working <cmath> headers, in which
// case something cleverer ought to be done here.
#include <math.h>
inline float       vcl_abs (float  x) { return (x >= 0.0f) ? x : -x; }
inline double      vcl_abs (double x) { return fabs (x); }
inline long double vcl_abs (long double x) { return fabs (x); }

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


#if defined (linux) && defined (__OPTIMIZE__)
// * avoid infinite recursion when calling vnl_math::isfinite().
// * avoid symbol in object file being called vnl_math::_isinf.
# undef isinf  
// * avoid that vnl_math::isnan is redefined in <math.h>.
# undef isnan  
#endif

#endif
