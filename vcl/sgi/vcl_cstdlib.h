#ifndef vcl_sgi_cstdlib_h_
#define vcl_sgi_cstdlib_h_
// .NAME vcl_cstdlib
// .INCLUDE vcl/vcl_cstdlib.h
// .FILE vcl_cstdlib.cxx

// Include system stdlib.h
#if defined(VCL_SGI_CC_720) || (defined(VCL_CXX_HAS_HEADER_CSTDLIB) && !VCL_CXX_HAS_HEADER_CSTDLIB)
# include <stdlib.h>
# define vcl_exit    exit
# define vcl_abort   abort
# define vcl_size_t  size_t
# define vcl_atoi    atoi
# undef vcl_abs
inline int  vcl_abs(int  x) { return x >= 0 ? x : -x; }
inline long vcl_abs(long x) { return x >= 0 ? x : -x; }

#else
# include <iso/vcl_cstdlib.h>
#endif

#endif // vcl_cstdlib_h_
