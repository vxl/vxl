#ifndef function_h_
#define function_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_utility.h>
#else
# ifdef __GNUC__
#  include_next <function.h>
#  define vcl_utility utility
# else
#  include <utility>
#  define vcl_utility std::utility
# endif
#endif

#endif
