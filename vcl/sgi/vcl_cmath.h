#ifndef vcl_sgi_cmath_h_
#define vcl_sgi_cmath_h_

#include <vcl/vcl_compiler.h>

#undef vcl_cmath_std

// 1. include system header
#if defined(VCL_SGI_CC_720)
# include <math.h>
#elif defined(VCL_CXX_HAS_HEADER_CMATH) && !VCL_CXX_HAS_HEADER_CMATH
# include <math.h> // e.g. SGI CC 7.30
# define vcl_cmath_std
#else // iso
# include <vcl/iso/vcl_cmath.h>
# define vcl_cmath_std
#endif


// 1.5 fix system header.
#if defined (linux) && defined (__OPTIMIZE__)
// * avoid infinite recursion when calling vnl_math::isfinite().
// * avoid symbol in object file being called vnl_math::_isinf.
# undef isinf  
// * avoid that vnl_math::isnan is redefined in <math.h>.
# undef isnan  
#endif


// 2. define vcl_abs() inline functions.
#if !defined(vcl_abs)
# if defined(VCL_SGI_CC_720)
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
#ifndef vcl_sqrt
# define vcl_sqrt sqrt
#endif
#define vcl_tan tan
#define vcl_tanh tanh
#endif

#undef vcl_cmath_std

#endif // vcl_cmath_h_
