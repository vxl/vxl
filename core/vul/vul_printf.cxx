// This is core/vul/vul_printf.cxx
//
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 03 Jul 97
//
//-----------------------------------------------------------------------------

#include <cstdarg>
#include <iostream>
#include <cstdio>
#include "vul_printf.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

std::ostream& vul_printf(std::ostream& s, char const * fmt, ...)
{
  char buf[65536];

  std::va_list ap;
  va_start(ap, fmt);
  std::vsprintf(buf, fmt, ap);
  va_end(ap);

  return s << buf;
}
