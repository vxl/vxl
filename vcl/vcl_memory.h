#ifndef vcl_memory_h_
#define vcl_memory_h_
/*
  fsm
*/

#include "vcl_compiler.h"

#if !VCL_USE_NATIVE_STL
# include "emulation/vcl_algorithm.h"
#elif defined(VCL_VC60)
# include "win32-vc60/vcl_memory.h"
#else
# include "iso/vcl_memory.h"
#endif

#endif // vcl_memory_h_
