#ifndef deque_h_
#define deque_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_deque.h>

#elif defined(VCL_GCC)
# include_next <deque.h>
# define vcl_deque deque

#else
# include <deque>
# define vcl_deque std::deque
#endif

#define VCL_DEQUE_INSTANTIATE \
extern "include vcl/vcl_deque.txx instead"

#endif
