// This is vcl/vcl_limits.h
#ifndef vcl_limits_h_
#define vcl_limits_h_

#include "vcl_compiler.h"


#if !VCL_CXX_HAS_HEADER_LIMITS || !VCL_USE_NATIVE_STL
# include  "emulation/vcl_limits.h"

#else
# include "iso/vcl_limits.h"
#endif

#endif // vcl_limits_h_
