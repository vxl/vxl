#ifndef deque_h_
#define deque_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_deque.h>
#else
# ifdef __GNUC__
#  include <deque.h>
#  define vcl_deque deque
# else
#  include <deque>
#  define vcl_deque std::deque
# endif
#endif

#endif
