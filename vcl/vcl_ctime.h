#ifndef vcl_ctime_h_
#define vcl_ctime_h_
/*
  fsm@robots.ox.ac.uk
*/

#include "vcl_compiler.h"

// Purpose: provide access to the <time.h> standard C header.

#if !VCL_CXX_HAS_HEADER_CTIME || defined(VCL_METRO_WERKS)
# include <time.h>
# define vcl_generic_ctime_STD /* */
# include "generic/vcl_ctime.h"
#elif defined(VCL_VC60)
# include "win32-vc60/vcl_ctime.h"
#elif defined(VCL_VC70)
# include "win32-vc70/vcl_ctime.h"
#else
# include "iso/vcl_ctime.h"
#endif

// ??
#ifndef VCL_WIN32
#include <sys/times.h>
#endif

#endif // vcl_ctime_h_
