#ifndef vcl_sunpro_cmath_h_
#define vcl_sunpro_cmath_h_

#include <vcl/vcl_compiler.h>

// 1. include system header
#if defined(VCL_CXX_HAS_HEADER_CMATH) && !VCL_CXX_HAS_HEADER_CMATH
# include <math.h> // e.g. SGI CC 7.30
#else // iso
# include <vcl/iso/vcl_cmath.h>
#endif

// 1.5 fix system header.
#if defined(VCL_SUNPRO_CC_50)
# include <math.h> // for the HUGE_VAL macro.
# undef M_PI_4     // avoid redef. it's non-iso anyway.
#endif

// define vcl_abs() inline functions.
#if !defined(vcl_abs)
# define vcl_abs std:: abs
#endif

// extensions.
#if defined(VCL_SUNPRO_CC_50)
#define VCL_NO_DRAND48
#endif

#endif // vcl_cmath_h_
