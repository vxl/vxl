#ifndef vcl_memory_h_
#define vcl_memory_h_
/*
  fsm
*/

#include "vcl_compiler.h"

#if defined(VCL_VC60)
# include "win32-vc60/vcl_memory.h"
#elif defined(VCL_GCC_295)
# include "gcc-295/vcl_memory.h"
#else

# include "iso/vcl_memory.h"

# if VCL_INCLUDE_CXX_0X
#  if VCL_MEMORY_HAS_SHARED_PTR
#   include "iso/vcl_memory_tr1.h"
#  elif VCL_TR1_MEMORY_HAS_SHARED_PTR
#   include "tr1/vcl_memory.h"
#  else
// This is where C++0x emulation goes when available
#error "****Error: shared_ptr emulation not available****"
#  endif // VCL_MEMORY_HAS_SHARED_PTR
# endif // VCL_INCLUDE_CXX_0X

#endif

#endif // vcl_memory_h_
