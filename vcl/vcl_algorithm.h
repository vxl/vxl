#ifndef vcl_algorithm_h_
#define vcl_algorithm_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_algorithm.h>
#define vcl_swap swap
#else
# ifdef __GNUC__
#  include <algo.h>
using std::swap;
#define vcl_swap swap
# else
// sunpro, win32
#  include <algorithm>
#  include <functional>
using std::swap;
#define vcl_swap swap
using std::copy;
# endif
#endif

#endif
