#ifndef vcl_cstdlib_h_
#define vcl_cstdlib_h_

#ifdef __GNUG__
#pragma interface "vcl_cstdlib.h"
#endif

// [26.5.4] In addition to the signatures from <stdlib.h> the C++
// header <cstdlib> adds the overloaded signatures :
//   long   abs(long);        // labs()
//   ldiv_t div(long, long);  // ldiv()

#include <vcl/vcl_compiler.h>

// Include system stdlib.h
#if defined(VCL_GCC_27) || defined(VCL_SGI_CC_720) || defined(VCL_VC60)
# include <stdlib.h>
# define vcl_exit    exit
# define vcl_abort   abort
# define vcl_size_t  size_t
#else
# include <vcl/iso/vcl_cstdlib.h>
#endif

#ifndef vcl_abs
# if defined(VCL_EGCS) || defined(VCL_GCC_295)
#  define vcl_abs abs

# elif defined(VCL_GCC_27) || defined(VCL_SGI_CC_720)
inline int vcl_abs(int x) { return x >= 0 ? x : -x; }
inline int vcl_abs(long x) { return x >= 0 ? x : -x; }
# else
#  define vcl_abs std::abs
# endif
#endif

//inline ldiv_t vcl_div(long x, long y) { return ldiv (x, y); }

#endif
