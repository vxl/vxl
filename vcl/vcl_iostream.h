#ifndef vcl_iostream_h_
#define vcl_iostream_h_

// .NAME vcl_iostream
// .INCLUDE vcl_iostream.h
// .SECTION Author
//    awf@robots.ox.ac.uk
// .SECTION Description
//  include compiler's <iostream.h> in a uniform way.
//  in particular, define the following
// \verbatim
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
//   vcl_cin
//   vcl_cout
//   vcl_cerr
// \endverbatim
//

// Include this to ensure the two are consistent.
#include <vcl_iosfwd.h>

// Notes to maintainers.
//   The purpose of this file is to repair broken iostream
// headers. Thus in conditional logic, the compilers that 
// behave in a non-standard way should be treated first, as
// special cases, and the #else arm should contain the 
// appropriate action for an ISO compiler.

// On win32, <iostream.h> contains old crufty iostreams and
// <iostream> contains new standard ones. There is no iosfwd
// for the old ones and <string> includes the new iostreams.
// So we must avoid the old ones at any price.

// ------------------------------------------------------------

#if defined(VCL_GCC_27) || defined(VCL_SGI_CC_720) || ((defined(VCL_EGCS) || defined(VCL_GCC_295)) && !defined(GNU_LIBSTDCXX_V3))
# include <iostream.h>
# define vcl_ios          ios
# define vcl_ios_in       ios::in
# define vcl_ios_out      ios::out
# define vcl_ios_ate      ios::ate
# define vcl_ios_app      ios::app
# define vcl_ios_trunc    ios::trunc
# define vcl_ios_binary   ios::binary
# define vcl_hex          hex
# define vcl_dec          dec
# define vcl_ws           ws
# define vcl_setprecision setprecision
# define vcl_endl         endl
# define vcl_flush        flush
# define vcl_streampos    streampos
# define vcl_streambuf    streambuf
# define vcl_streamsize   streamsize
# define vcl_cin          cin
# define vcl_cout         cout
# define vcl_cerr         cerr

#else // -------------------- ISO
# include "iso/vcl_iostream.h"
#endif

// -------------------- miscellaneous fixes which can go at the end:

#if defined(VCL_SGI_CC_720)
inline istream& operator>>(istream& s, signed char& c) 
{
  char i;
  s >> i;
  c = i;
  return s;
}
// SGI CC has no ios::bin, but since UNIX makes no distinction
// between binary and non-binary, 0 works just as well.
# undef  vcl_ios_binary
# define vcl_ios_binary   ios::open_mode(0)/*ios::bin*/
#endif

// Need std::ios::nocreate to avoid creating an empty file on 
// attempts to read a non-existent one. Don't we? -- fsm
#if defined(VCL_VC50)
# undef  vcl_ios_in
# define vcl_ios_in      (std::ios::in | std::ios::nocreate)
#endif

#if defined(VCL_SGI_CC_720)
// This is not there.
# undef  vcl_streamsize
# define vcl_streamsize   unsigned
#endif

#if defined(VCL_GCC_27)
// 2.7.2.3 has both "bin" and "binary", but 2.7.2 has
// only "bin". so we use "bin". fsm@robots.
# undef  vcl_ios_binary
# define vcl_ios_binary   ios::bin
#endif

#endif // vcl_iostream_h_
