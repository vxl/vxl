#ifndef vcl_strstream_h_
#define vcl_strstream_h_
/*
  fsm@robots.ox.ac.uk
*/

// this is to get the vcl_ios_* macros.
#include <vcl/vcl_iostream.h>

// include compiler header.
#ifdef VCL_WIN32
#include <strstream>
using std :: ostrstream;
using std :: istrstream;

#else
#include <strstream.h>
#endif

#endif
