#ifndef vcl_stlfwd_h_
#define vcl_stlfwd_h_
#ifdef __GNUC__
#pragma interface
#endif

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_stlfwd.h>
#else
# include <vcl/vcl_alloc.h>
# include <vcl/vcl_vector.h>
# include <vcl/vcl_stack.h>
# include <vcl/vcl_list.h>
# include <vcl/vcl_map.h>
# include <vcl/vcl_set.h>
# include <vcl/vcl_stack.h>
# include <vcl/vcl_queue.h>
# include <vcl/vcl_deque.h>
#endif

#endif
