#ifndef vcl_csetjmp_h_
#define vcl_csetjmp_h_
/*
  Peter.Vanroose@esat.kuleuven.ac.be
*/

/* This should define C-style stack unwinding */

#include "vcl_compiler.h"

// SunPro 5.0's <csetjmp> is broken.
#if !VCL_CXX_HAS_HEADER_CSETJMP || defined(VCL_SUNPRO_CC_50)
# include <setjmp.h>
# define vcl_generic_csetjmp_STD /* */
# include "generic/vcl_csetjmp.h"
#else
# include "iso/vcl_csetjmp.h"
#endif

// In ISO C, setjmp() is a macro. However, many people
// don't know this so we provide another macro here in
// order to avoid confusing people who think they want
// to use std::setjmp() or ::setjmp().
#define vcl_setjmp setjmp

#endif // vcl_csetjmp_h_
