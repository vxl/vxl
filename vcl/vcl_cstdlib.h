#ifndef vcl_cstdlib_h_
#define vcl_cstdlib_h_

#ifdef __GNUG__
#pragma interface "vcl_cstdlib.h"
#endif

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_stlconf.h>
#else
extern "C++" {
# if _G_HAS_LABS
  inline long   abs(long x)		{ return labs (x); }
# else
  inline long   abs(long x)		{ return x >= 0 ? x : -x; }
# endif
  //inline ldiv_t div(long x, long y)	{ return ldiv (x, y); }
}
#endif

// Need to #include this in both cases. It's the client's reason
// for #including vcl_cstdlib.h in the first place.
#include <stdlib.h>

#endif
