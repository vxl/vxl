#ifndef vcl_list_h_
#define vcl_list_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_list.h>

#elif defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3)
# include <list.h>
# define vcl_list list

#else
# include <vcl/iso/vcl_list.h>
#endif

#define VCL_LIST_INSTANTIATE \
extern "include vcl/vcl_list.txx instead"

#endif
