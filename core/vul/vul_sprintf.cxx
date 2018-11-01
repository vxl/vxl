// This is core/vul/vul_sprintf.cxx
//
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 08 Aug 96
//
//-----------------------------------------------------------------------------

#include <cstdarg>
#include <cstring>
#include <iostream>
#include <cstdio>
#include "vul_sprintf.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#undef vsprintf // this works around a bug in libintl.h

vul_sprintf::vul_sprintf(char const *fmt, ...) : std::string("")
{
  std::va_list ap;
  va_start(ap, fmt);

  char s[65536];
  std::vsprintf(s, fmt, ap);
  if (std::strlen(s) >= sizeof s)
    std::cerr << __FILE__ ": WARNING! Possible memory corruption after call to vsprintf()\n";
  std::string::operator=(s);

  va_end(ap);
}

std::ostream & operator<<(std::ostream &os,const vul_sprintf& s)
{
  return os << (char const*)s;
}

//--------------------------------------------------------------------------------

#ifdef RUNTEST
main()
{
  std::cout << vul_sprintf("fred%d\n", 3);
  std::string fmt("foobar%d\n");
  std::cout << vul_sprintf(fmt, 4);
}
#endif
