#ifndef vcl_cmath_h_
#define vcl_cmath_h_
// .NAME vcl_cmath.h
// .INCLUDE vcl/vcl_cmath.h
// .FILE vcl_cmath.cxx

#ifdef __GNUG__
#pragma interface
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

#undef vcl_cmath_std

// 1. include system header
#if defined(VCL_GCC_27)
# include <math.h>
#elif defined(VCL_SGI_CC_720)
# include <math.h>
#elif defined(VCL_VC60)
# include <math.h>
#elif defined(VCL_EGCS) || (defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3))
# include <cmath> // got the right prototypes, but not in namespace std::
#elif defined(VCL_CXX_HAS_HEADER_CMATH) && !VCL_CXX_HAS_HEADER_CMATH
# include <math.h> // e.g. SGI CC 7.30
# define vcl_cmath_std
#else // iso
# include <vcl/iso/vcl_cmath.h>
# define vcl_cmath_std
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
#if defined(VCL_GCC_295) && defined(GNU_LIBSTDCXX_V3)
# undef  vcl_abs
# define vcl_abs abs
using std::abs;

#elif !defined(vcl_abs)
# if defined(VCL_EGCS) || defined(VCL_GCC_295)
#  define vcl_abs  abs

# elif defined(VCL_GCC_27) || defined(VCL_SGI_CC_720) || defined(VCL_VC60)
inline float       vcl_abs (float  x) { return (x >= 0.0f) ? x : -x; }
inline double      vcl_abs (double x) { return fabs (x); }
inline long double vcl_abs (long double x) { return fabs (x); }

# else
#  define vcl_abs std:: abs
# endif
#endif

// 3. Define remaining math functions
// Don't do this for iso as it has already been done in iso/vcl_cmath.h
#ifndef vcl_cmath_std
#define vcl_acos acos
#define vcl_asin asin
#define vcl_atan atan
#define vcl_atan2 atan2
#define vcl_ceil ceil
#define vcl_cos cos
#define vcl_cosh cosh
#define vcl_exp exp
#define vcl_fabs fabs
#define vcl_floor floor
#define vcl_fmod fmod
#define vcl_frexp frexp
#define vcl_ldexp ldexp
#define vcl_log log
#define vcl_log10 log10
#define vcl_modf modf
#define vcl_pow pow
#define vcl_pow pow
#define vcl_sin sin
#define vcl_sinh sinh
#define vcl_sqrt sqrt
#define vcl_tan tan
#define vcl_tanh tanh
#endif

#undef vcl_cmath_std

// vcl_min/vcl_max moved to vcl_algorithm.h


// 3. extensions.
#if defined(WIN32) || defined(VCL_SUNPRO_CC_50)
#define VCL_NO_DRAND48
#endif

#endif
