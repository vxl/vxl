// This is core/vul/vul_sprintf.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 08 Aug 96
//
//-----------------------------------------------------------------------------

#include "vul_sprintf.h"

#include <vcl_cstdarg.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>
#undef vsprintf // this works around a bug in libintl.h
#include <vcl_cstdio.h> // for vsprintf()

vul_sprintf::vul_sprintf(char const *fmt, ...) : vcl_string("")
{
  vcl_va_list ap;
  va_start(ap, fmt);

  char s[65536];
  vcl_vsprintf(s, fmt, ap);
  if (vcl_strlen(s) >= sizeof s)
    vcl_cerr << __FILE__ ": WARNING! Possible memory corruption after call to vsprintf()\n";
  vcl_string::operator=(s);

  va_end(ap);
}

vcl_ostream & operator<<(vcl_ostream &os,const vul_sprintf& s)
{
  return os << (char const*)s;
}

//--------------------------------------------------------------------------------

#ifdef RUNTEST
main()
{
  vcl_cout << vul_sprintf("fred%d\n", 3);
  vcl_string fmt("foobar%d\n");
  vcl_cout << vul_sprintf(fmt, 4);
}
#endif
