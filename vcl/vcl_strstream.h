#ifndef vcl_strstream_h_
#define vcl_strstream_h_
/*
  fsm@robots.ox.ac.uk
*/

// this is to get the vcl_ios_* macros.
#include <vcl/vcl_iostream.h>

// include compiler header.
#if defined(VCL_GCC_27) || defined(VCL_GCC_WITH_LIBSTDCXX_V2) || defined(VCL_SGI_CC)
# include <strstream.h>
#else // -------------------- ISO
# include <strstream>
using std :: ostrstream;
using std :: istrstream;
#endif

#endif
