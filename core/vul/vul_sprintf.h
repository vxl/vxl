// This is core/vul/vul_sprintf.h
#ifndef vul_sprintf_h_
#define vul_sprintf_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief creates a formatted ANSI C++ string
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   08 Aug 96
//
// \verbatim
//  Modifications
//   10 June 1999 fsm removed constructor from 'const vcl_string &' and
//                    changed remaining constructors to use do_vul_sprintf().
//   Peter Vanroose   27/05/2001: Corrected the documentation
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <vcl_iosfwd.h>

//: C++ conforming replacement to the ANSI C functions sprintf and printf.
// vul_sprintf works in the same way as sprintf but is itself an ANSI C++ string
// which can either be kept or output directly using streams  e.g.
// \code
// vcl_cerr << vul_sprintf("int %d, float %f ", 1, 3.14)
//          << bigobject << vcl_endl;
// \endcode

struct vul_sprintf : public vcl_string
{
  // ISO C++ does not allow reference types or structure types for the
  // argument preceding ... in a function taking a variable number of
  // parameters.
  // So we can't have any of these constructors:
  //   vul_sprintf(vcl_string const& fmt, ...);
  //   vul_sprintf(vcl_string fmt, ...);
  vul_sprintf(char const *fmt, ...);

#ifndef VCL_WIN32
  // assignment
  vul_sprintf& operator=(vcl_string const& s)
  { vcl_string::operator=(s); return *this; }
  vul_sprintf& operator=(char const* s)
  { vcl_string::operator=(s); return *this; }
#endif

  operator char const* () const { return c_str(); }
};

vcl_ostream& operator<<(vcl_ostream &os, const vul_sprintf& s);

#endif // vul_sprintf_h_
