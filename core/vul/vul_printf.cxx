// This is vxl/vul/vul_printf.cxx

//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//

// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 03 Jul 97
//
//-----------------------------------------------------------------------------

#include "vul_printf.h"
 
#include <vcl_cstdarg.h>
#include <vcl_cstdio.h>
#include <vcl_iostream.h>
 
vcl_ostream& vul_printf(vcl_ostream& s, char const * fmt, ...)
{
  char buf[65536];
  
  va_list ap;
  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  va_end(ap);
  
  return s << buf;
}
