#ifndef vcl_fstream_h_
#define vcl_fstream_h_
/*
  fsm@robots.ox.ac.uk
*/

// this is to get the vcl_ios_* macros.
#include <vcl/vcl_iostream.h>

#if (defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3)) || defined(VCL_SGI_CC_720)
# include <fstream.h>

#else  // -------------------- ISO
# include <vcl/iso/vcl_fstream.h>
#endif

#endif // vcl_fstream_h_
