#ifndef vbl_sprintf_h_
#define vbl_sprintf_h_
// This is vxl/vbl/vbl_sprintf.h
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief creates a formatted ANSI C++ string
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   08 Aug 96
//
// \verbatim
// Modifications:
// 10 June 1999 fsm@robots removed constructor from 'const vcl_string &' and
//              changed remaining constructors to use do_vbl_sprintf().
// Peter Vanroose   27/05/2001: Corrected the documentation
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <vcl_iosfwd.h>

//: C++ conforming replacement to the ANSI C functions sprintf and printf.
// vbl_sprintf works in the same way as sprintf but is itself an ANSI C++ string
// which can either be kept or output directly using streams  e.g.
// \verbatim
// cerr << vbl_sprintf("int %d, float %f ", 1, 3.14)
//      << bigobject << endl;
// \endverbatim

struct vbl_sprintf : vcl_string
{
  // ISO C++ does not allow reference types or structure types for the
  // argument preceding ... in a function taking variable a number of
  // paramaters.
  // So we can't have any of these constructors:
  //   vbl_sprintf(vcl_string const& fmt, ...);
  //   vbl_sprintf(vcl_string fmt, ...);
  vbl_sprintf(char const *fmt, ...);

#ifndef VCL_WIN32
  // assignment
  vbl_sprintf& operator=(vcl_string const& s)
  { vcl_string::operator=(s); return *this; }
  vbl_sprintf& operator=(char const* s)
  { vcl_string::operator=(s); return *this; }
#endif

  operator char const* () const { return c_str(); }
};

vcl_ostream& operator<<(vcl_ostream &os, const vbl_sprintf& s);

#endif // vbl_sprintf_h_
