#ifndef vcl_streambuf_h_
#define vcl_streambuf_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

// Purpose: to #define vcl_streambuf to std::streambuf on 
// conforming implementations and to fix the non-conforming 
// ones.

#include <vcl/vcl_iostream.h> // to get vcl_ios_*

#ifdef VCL_WIN32
# include <streambuf>
# define vcl_streambuf std::streambuf

#elif defined(VCL_GCC)
# include <streambuf.h>
# define vcl_streambuf streambuf


#else
# include <streambuf>
# define vcl_streambuf std::streambuf
#endif

#endif
