#ifndef vcl_queue_h_
#define vcl_queue_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_queue.h>

#elif defined(VCL_GCC_which_one_question_mark)
// egcs and 2.95 have <queue> -- fsm
# include <stack.h>
# define vcl_queue queue

#else
# include <vcl/iso/vcl_queue.h>
#endif

#define VCL_QUEUE_INSTANTIATE(T) extern "you must #include vcl/vcl_queue.txx"

#endif
