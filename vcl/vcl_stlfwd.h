#ifndef vcl_stlfwd_h_
#define vcl_stlfwd_h_
#ifdef __GNUC__
#pragma interface
#endif

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_stlfwd.h>
#else
#include <vcl_alloc.h>
#include <vcl_vector.h>
#include <vcl_stack.h>
#include <vcl_list.h>
#include <vcl_map.h>
#include <vcl_multimap.h>
#include <vcl_set.h>
#include <vcl_stack.h>
#include <vcl_queue.h>
#endif
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vcl_stlfwd.
