#ifndef vbl_sprintf_h_
#define vbl_sprintf_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	vbl_sprintf - creates a formatted ANSI C++ string
// .LIBRARY	vbl
// .HEADER	vxl package
// .INCLUDE	vbl/vbl_sprintf.h
// .FILE	vbl_sprintf.cxx
//
// .SECTION Description
//
// vbl_sprintf is a C++ conforming replacement to the ANSI C functions sprintf
// and printf.
// It works in the same way as sprintf but is itself an ANSI C++ string
// which can either be kept or output directly using streams  e.g.
// \verbatim
// cerr << vbl_sprintf("int %d, float %f ", 1, 3.14)
//      << bigobject << endl;
// \endverbatim
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 08 Aug 96
//
// .SECTION Modifications:
// 10 June 1999 fsm@robots removed constructor from 'const vcl_string &' and
//              changed remaining constructors to use do_vbl_sprintf().
//
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <vcl_iosfwd.h>

class vbl_sprintf : public vcl_string {
public:
  typedef vcl_string super;
  // ISO C++ does not allow reference type for the argument preceding ...
  // If you can't afford to pass a string by value, use string::c_str()
  //vbl_sprintf(const vcl_string& fmt, ...);

  // Nor does it allow parmN to be a structure type (in most cases) and
  // while that may work on many platforms, it breaks on native SGI.
  // So it's not allowed.
  //vbl_sprintf(vcl_string fmt, ...); // va_start() broken for parmN of type string.

  // constructor/destructor
  vbl_sprintf(const char *fmt, ...);
 ~vbl_sprintf();

#ifndef VCL_WIN32
  // assignment
  vbl_sprintf& operator=(const vcl_string& s) { vcl_string::operator=(s); return *this; }
  vbl_sprintf& operator=(const char* s) { vcl_string::operator=(s); return *this; }
#endif

  // cast to const char *
  operator const char* () const;

  // hacks
#if defined(VCL_GCC_27)
  bool operator==(const vcl_string &s) const { return ((const vcl_string &)*this) == s; }
#endif
};

ostream& operator<<(ostream &os, const vbl_sprintf& s);

#endif // vbl_sprintf_h_
