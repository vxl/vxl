//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// Class: vbl_printf
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 03 Jul 97
//
//-----------------------------------------------------------------------------

#include "vbl_printf.h"
 
#include <vcl_cstdarg.h>
#include <vcl_cstdio.h>
#include <vcl_iostream.h>
 
vcl_ostream& vbl_printf(vcl_ostream& s, const char * fmt, ...)
{
  char buf[65536];
  
  va_list ap;
  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  va_end(ap);
  
  return s << buf;
}
