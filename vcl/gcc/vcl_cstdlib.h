#ifndef vcl_gcc_cstdlib_h_
#define vcl_gcc_cstdlib_h_
// .NAME vcl_cstdlib
// .INCLUDE vcl/vcl_cstdlib.h
// .FILE vcl_cstdlib.cxx

#ifdef __GNUG__
#pragma interface
#endif

// Include system stdlib.h
#if defined(VCL_CXX_HAS_HEADER_CSTDLIB) && !VCL_CXX_HAS_HEADER_CSTDLIB
# include <stdlib.h>
# define vcl_exit    exit
# define vcl_abort   abort
# define vcl_size_t  size_t
# define vcl_atoi    atoi
inline int  vcl_abs(int  x) { return x >= 0 ? x : -x; }
inline long vcl_abs(long x) { return x >= 0 ? x : -x; }
#define vcl_abs vcl_abs

#else
# include <iso/vcl_cstdlib.h>
#endif

#endif // vcl_cstdlib_h_
