#ifndef vcl_cfloat_h_
#define vcl_cfloat_h_
//:
// \file
// \author Peter.Vanroose@esat.kuleuven.ac.be
// This should define C-style numeric floating point macros

#include "vcl_compiler.h"

#if !VCL_CXX_HAS_HEADER_CFLOAT
# include <float.h>
#else
# include "iso/vcl_cfloat.h"
#endif

#ifndef MAXFLOAT
# ifdef FLT_MAX
#  define MAXFLOAT FLT_MAX
# endif
#endif
#ifndef MAXDOUBLE
# ifdef DBL_MAX
#  define MAXDOUBLE DBL_MAX
# endif
#endif
#ifndef MINFLOAT
# ifdef FLT_MIN
#  define MINFLOAT FLT_MIN
# endif
#endif
#ifndef MINDOUBLE
# ifdef DBL_MIN
#  define MINDOUBLE DBL_MIN
# endif
#endif
#ifndef MAXFLOAT
# include <vcl_cmath.h>
#endif
#ifndef MAXFLOAT
# include <values.h>
#endif
#ifndef MAXFLOAT
# include <limits.h>
#endif
#ifndef MAXFLOAT
# define MAXFLOAT 3.40282346638528860e38F
#endif
#ifndef MAXDOUBLE
# define MAXDOUBLE 1.7976931348623157e308
#endif
#ifndef MINFLOAT
# define MINFLOAT 1.1754943508222875e-38F
#endif
#ifndef MINDOUBLE
# define MINDOUBLE 2.2250738585072014e-308
#endif
#ifndef FLT_MAX
# define FLT_MAX MAXFLOAT
#endif
#ifndef DBL_MAX
# define DBL_MAX MAXDOUBLE
#endif
#ifndef FLT_MIN
# define FLT_MIN MINFLOAT
#endif
#ifndef DBL_MIN
# define DBL_MIN MINDOUBLE
#endif

#endif // vcl_cfloat_h_
