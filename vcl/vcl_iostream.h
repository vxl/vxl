#ifndef vcl_iostream_h_
#define vcl_iostream_h_

// Author: awf@robots.ox.ac.uk
// Summary: include compiler's <iostream.h> in a uniform way.
//          in particular, defined the following
//   ostream
//   ostream_iterator
//   vcl_ios_X (as in ios::X)
//   operator>>(ostream &, T &) for T in {signed char, bool}

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_iosfwd.h>

// on win32, iostream.h contains old crufty iostreams, 
// iostream contains new standard ones.  There is no iosfwd for
// the old ones.
// on win32, string includes the new iostreams.  we must not use the old ones.

#ifdef VCL_WIN32
#include <iostream>
// in vcl_iosfwd.h using std :: ostream; 
using std :: ios;
using std :: flush;
using std :: cin;
using std :: cout;
using std :: cerr;
using std :: endl;
using std :: streampos;
using std :: streambuf; // added by Peter Vanroose
#else

#include <iostream.h>

#endif

#if defined(VCL_SGI_CC)
inline istream& operator>>(istream& s, signed char& c) {
  char i;
  s >> i;
  c = i;
  return s;
}
#endif

#if defined(VCL_VC50) || defined(VCL_VC60)

#endif

// fsm@robots. Added the following vcl_ios_* macros to fix win32 
// and sgi native. It appears that the following two are non-iso
// extensions, so don't put them in even if they appear in your
// C++ books (such as "The C++ Programming Language") :
//# define vcl_ios_nocreate  ios::nocreate
//# define vcl_ios_noreplace ios::noreplace
//  I had to use the native forms on WIN32, though, to avoid 
// creating an empty file when trying to read a non-existent one,
// but that's allowed.

#if defined(VCL_SGI_CC_7)
# define vcl_ios_in     ios::in
# define vcl_ios_out    ios::out
# define vcl_ios_ate    ios::ate
# define vcl_ios_app    ios::app
# define vcl_ios_trunc  ios::trunc
# define vcl_ios_binary ios::bin                  //

#elif defined(VCL_WIN32)
# define vcl_ios_in     ios::in 
# define vcl_ios_out    ios::out
# define vcl_ios_ate    ios::ate
# define vcl_ios_app    ios::app
# define vcl_ios_trunc  ios::trunc
# define vcl_ios_binary ios::binary

#else
# define vcl_ios_in     ios::in
# define vcl_ios_out    ios::out
# define vcl_ios_ate    ios::ate
# define vcl_ios_app    ios::app
# define vcl_ios_trunc  ios::trunc
# define vcl_ios_binary ios::binary
#endif

// moved here from vil_stream_fstream.cxx. streamsize added
#if defined(VCL_SUNPRO_CC) || defined(VCL_WIN32)
# define vcl_streampos  std::streampos
# define vcl_streambuf  std::streambuf
# define vcl_streamsize std::streamsize
#elif defined(VCL_SGI_CC)
# define vcl_streampos  streampos
# define vcl_streambuf  streambuf
# define vcl_streamsize unsigned
#else
# define vcl_streampos  streampos
# define vcl_streambuf  streambuf
# define vcl_streamsize streamsize
#endif

#endif
