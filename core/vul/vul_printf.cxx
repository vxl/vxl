// This is core/vul/vul_printf.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 03 Jul 97
//
//-----------------------------------------------------------------------------

#include "vul_printf.h"

#include <vcl_cstdarg.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h> // for vsprintf()

vcl_ostream& vul_printf(vcl_ostream& s, char const * fmt, ...)
{
  char buf[65536];

  vcl_va_list ap;
  va_start(ap, fmt);
  vcl_vsprintf(buf, fmt, ap);
  va_end(ap);

  return s << buf;
}
