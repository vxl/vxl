#ifndef vcl_iomanip_h_
#define vcl_iomanip_h_
/*
  fsm@robots.ox.ac.uk
*/

#include "vcl_compiler.h"
// NB. we allow both 'blah' and 'vcl_blah' to mean 'std::blah'.

#undef vcl_setw

#if (defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3)) || defined(VCL_SGI_CC_720)
# include <iomanip.h>
# define vcl_setw     ::setw

#else
# include "iso/vcl_iomanip.h"
#endif

#endif // vcl_iomanip_h_
