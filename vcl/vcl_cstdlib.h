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
#include <stdlib.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_stlconf.h>
#else
# if _G_HAS_LABS
inline long   vcl_abs(long x)         { return labs (x); }
# else
inline long   vcl_abs(long x)         { return x >= 0 ? x : -x; }
# endif
inline ldiv_t vcl_div(long x, long y) { return ldiv (x, y); }
#endif

#endif
