#ifndef vcl_list_h_
#define vcl_list_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_list.h>
#else
# ifdef __GNUC__
#  include_next <list.h>
#  define vcl_list list
# else
// Native STL, not GCC:
#  include <list>
#  define vcl_list std::list
# endif
#endif

#define VCL_LIST_INSTANTIATE \
extern "include vcl/vcl_list.txx instead"

#endif
