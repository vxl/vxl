#ifndef vcl_cstdarg_h_
#define vcl_cstdarg_h_
/*
  fsm
*/

#include "vcl_compiler.h"

#if !VCL_CXX_HAS_HEADER_CSTDARG
#  include <stdarg.h>
#  define vcl_va_list va_list
#else
#  include "iso/vcl_cstdarg.h"
#endif

#endif // vcl_cstdarg_h_
