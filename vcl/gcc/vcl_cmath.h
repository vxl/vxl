#ifndef vcl_gcc_cmath_h_
#define vcl_gcc_cmath_h_

#ifdef __GNUG__
#pragma interface
#endif

// #include <vcl_compiler.h> // not needed, as this file should be included through ../vcl_cmath.h

#undef vcl_cmath_std

// 1. include system header
#if defined(VCL_GCC_27)
# include <math.h>
#elif defined(VCL_EGCS) || (defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3))
# include <cmath> // got the right prototypes, but not in namespace std::
#elif defined(VCL_CXX_HAS_HEADER_CMATH) && !VCL_CXX_HAS_HEADER_CMATH
# include <math.h> // e.g. SGI CC 7.30
# define vcl_cmath_std
#else // iso
# include "../iso/vcl_cmath.h"
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
#if defined(VCL_GCC_295) && defined(GNU_LIBSTDCXX_V3)
# undef  vcl_abs
# define vcl_abs abs
using std::abs;

#elif !defined(vcl_abs)
# if defined(VCL_EGCS) || defined(VCL_GCC_295)
#  define vcl_abs  abs

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
