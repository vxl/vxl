// This is vcl/vcl_cstdlib.h
#ifndef vcl_cstdlib_h_
#define vcl_cstdlib_h_
// :
// \file
//
// [26.5.4] In addition to the signatures from <stdlib.h> the C++
// header <cstdlib> adds the overloaded signatures :
//   long   abs(long);        // labs()
//   ldiv_t div(long, long);  // ldiv()
//
// NB: size_t is declared in <cstddef>, not <cstdlib>

#include "vcl_compiler.h"

#if defined(VCL_VC_9) // need to handle abs(__int64) correctly
#  include "win32-vc9/vcl_cstdlib.h"
#else
#  include "iso/vcl_cstdlib.h"
#endif

#endif // vcl_cstdlib_h_
