#ifndef vcl_fstream_h_
#define vcl_fstream_h_
/*
  fsm@robots.ox.ac.uk
*/

// this is to get the vcl_ios_* macros.
#include <vcl/vcl_iostream.h>

#if defined(VCL_GCC_27) || defined(VCL_GCC_WITH_LIBSTDCXX_V2)
# include <fstream.h>

#else  // -------------------- ISO
# include <fstream>
using std :: fstream;
using std :: ofstream;
using std :: ifstream;
#endif

#endif
