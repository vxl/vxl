#ifndef set_h_
#define set_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_set.h>
#else
# ifdef __GNUC__
#  include <set>
#  define vcl_set set
# else
#  include <set>
#  define vcl_set std::set
# endif
#endif

#endif
