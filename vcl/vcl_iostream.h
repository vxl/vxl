#ifndef vcl_iostream_h_
#define vcl_iostream_h_

// Author: awf@robots.ox.ac.uk
// Summary: include compiler's <iostream.h> in a uniform way.
//          in particular, define the following
//   vcl_ostream
//   vcl_ios_X (as in ios::X)
//   operator>>(vcl_ostream &, T &) for T in {signed char, bool}
//   vcl_hex
//   vcl_dec
//   vcl_ws
//   vcl_setprecision
//   vcl_streampos
//   vcl_streambuf
//   vcl_streamsize
//

// Include this to ensure the two are consistent.
#include <vcl/vcl_iosfwd.h>

// Notes to maintainers.
//   The purpose of this file is to repair broken iostream
// headers. Thus in conditional logic, the compilers that 
// behave in a non-standard way should be treated first, as
// special cases, and the #else arm should contain the 
// appropriate action for an ISO compiler.

// ------------------------------------------------------------
// 1. include the system header.
#if defined(VCL_GCC) || defined(VCL_SGI_CC)
# include <iostream.h>
#else // -------------------- ISO
// On win32, <iostream.h> contains old crufty iostreams and
// <iostream> contains new standard ones. There is no iosfwd
// for the old ones and <string> includes the new iostreams.
// So we must avoid the old ones at any price.
# include <iostream>
#endif

// ------------------------------------------------------------
// 2. define vcl_ios_*
// Note that the ios::nocreate and ios::noreplace are non-ISO
// extensions, so don't put them in even though they appear
// in early versions of "The C++ Programming Language".
#if defined(VCL_SGI_CC)
# define vcl_ios_in     ios::in
# define vcl_ios_out    ios::out
# define vcl_ios_ate    ios::ate
# define vcl_ios_app    ios::app
# define vcl_ios_trunc  ios::trunc
// SGI CC has no ios::bin, but since UNIX makes no distinction
// between binary and non-binary, 0 works just as well.
# define vcl_ios_binary ios::open_mode(0)/*ios::bin*/

#elif defined(VCL_GCC_27)
# define vcl_ios_in     ios::in
# define vcl_ios_out    ios::out
# define vcl_ios_ate    ios::ate
# define vcl_ios_app    ios::app
# define vcl_ios_trunc  ios::trunc
// 2.7.2.3 has both "bin" and "binary", but 2.7.2 has
// only "bin". so we use "bin". fsm@robots.
# define vcl_ios_binary ios::bin

#elif defined(VCL_VC50)
// Need ios::nocreate to avoid creating an empty file on 
// attempts to read a non-existent one. Don't we? -- fsm
# define vcl_ios_in     (ios::in | ios::nocreate)
# define vcl_ios_out    ios::out
# define vcl_ios_ate    ios::ate
# define vcl_ios_app    ios::app
# define vcl_ios_trunc  ios::trunc
# define vcl_ios_binary ios::binary

#elif defined(VCL_VC60)
# define vcl_ios_in     ios::in
# define vcl_ios_out    ios::out
# define vcl_ios_ate    ios::ate
# define vcl_ios_app    ios::app
# define vcl_ios_trunc  ios::trunc
# define vcl_ios_binary ios::binary

#else // -------------------- ISO
# define vcl_ios_in     ios::in
# define vcl_ios_out    ios::out
# define vcl_ios_ate    ios::ate
# define vcl_ios_app    ios::app
# define vcl_ios_trunc  ios::trunc
# define vcl_ios_binary ios::binary
#endif

// ------------------------------------------------------------
// 3. define vcl_hex, vcl_dec, vcl_ws and common types.
#if defined(VCL_SGI_CC)
# define vcl_hex          hex
# define vcl_dec          dec
# define vcl_ws           ws
# define vcl_setprecision setprecision
# define vcl_endl         endl
# define vcl_streampos    streampos
# define vcl_streambuf    streambuf
# define vcl_streamsize   unsigned

#elif defined(VCL_GCC)
# define vcl_hex          hex
# define vcl_dec          dec
# define vcl_ws           ws
# define vcl_setprecision setprecision
# define vcl_endl         endl
# define vcl_streampos    streampos
# define vcl_streambuf    streambuf
# define vcl_streamsize   streamsize

#else // -------------------- ISO
# define vcl_hex          std::hex
# define vcl_dec          std::dec
# define vcl_ws           std::ws
# define vcl_setprecision std::setprecision
# define vcl_endl         std::endl
# define vcl_streampos    std::streampos
# define vcl_streambuf    std::streambuf
# define vcl_streamsize   std::streamsize
#endif

#define vcl_cin cin
#define vcl_cout cout
#define vcl_cerr cerr

// ------------------------------------------------------------
// 4. various

#if defined(VCL_SGI_CC)
inline istream& operator>>(istream& s, signed char& c) {
  char i;
  s >> i;
  c = i;
  return s;
}

#elif defined(VCL_GCC)
// Nothing really to do here, except avoid the using 
// statements for strictly ISO compilers.

#else // -------------------- ISO
using std :: ios;
using std :: flush;
using std :: cin;
using std :: cout;
using std :: cerr;
using std :: endl;
using std :: streampos;
using std :: streambuf;
#endif

#endif
