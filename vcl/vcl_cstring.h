#ifndef vcl_cstring_h_
#define vcl_cstring_h_

#include "vcl_compiler.h"

//#if defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3)
//# include <string.h>
//# define vcl_memcpy ::memcpy
#if defined(VCL_SGI_CC_720)
# include <string.h>
# define vcl_generic_cstring_STD /* */
# include "generic/vcl_cstring.h"
#else
# include "iso/vcl_cstring.h"
#endif

#endif // vcl_cstring_h_
