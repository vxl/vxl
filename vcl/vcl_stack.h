#ifndef stack_h_
#define stack_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_stack.h>
#else
# ifdef __GNUC__
#  include_next <stack.h>
#  define vcl_stack stack
# else
#  include <stack>
#  define vcl_stack std::stack
# endif
#endif

#endif
