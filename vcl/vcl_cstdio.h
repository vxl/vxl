#ifndef vcl_cstdio_h_
#define vcl_cstdio_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include "vcl_compiler.h"

#if defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3)
# include <stdio.h>
#elif defined(VCL_SGI_CC_720)
# include <stdio.h>
#elif defined(VCL_SUNPRO_CC_50)
# include <iosfwd> // <cstdio> breaks <iosfwd>
# include "iso/vcl_cstdio.h"
#else
# include "iso/vcl_cstdio.h"
#endif

#endif // vcl_cstdio_h_
