//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vbl_printf.h"
#endif
//
// Class: vbl_printf
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 03 Jul 97
// Modifications:
//   970703 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "vbl_printf.h"
 
#include <stdarg.h>
#include <stdio.h>
#include <vcl/vcl_iostream.h>
 
ostream& vbl_printf(ostream& s, const char * fmt, ...)
{
  char buf[65536];
  
  va_list ap;
  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  va_end(ap);
  
  return s << buf;
}
