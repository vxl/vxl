#ifndef queue_h_
#define queue_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_queue.h>

#elif defined(VCL_GCC)
# include <stack.h>
# define vcl_queue queue

#else
# include <queue>
# define vcl_queue std::queue
#endif

#define VCL_QUEUE_INSTANTIATE extern "you must #include vcl/vcl_queue.txx"

#endif
