#ifndef vcl_fstream_h_
#define vcl_fstream_h_
/*
  fsm@robots.ox.ac.uk
*/

// this is to get the vcl_ios_* macros.
#include <vcl/vcl_iostream.h>

// include compiler header.
#if defined(VCL_WIN32) && defined(VCL_USE_NATIVE_STL)
#include <fstream>
using std :: fstream;
using std :: ofstream;
using std :: ifstream;
#else
#include <fstream.h>
#endif

#endif
