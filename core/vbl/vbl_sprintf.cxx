//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// .NAME vbl_sprintf
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 08 Aug 96
//
//-----------------------------------------------------------------------------

#include "vbl_sprintf.h"

#include <vcl_cstdarg.h>
#include <vcl_cstdio.h>

#include <vcl_iostream.h>

vbl_sprintf::vbl_sprintf(char const *fmt, ...) : vcl_string("")
{
  va_list ap;
  va_start(ap, fmt);
  
  char s[65536];
  vsprintf(s, fmt, ap);
  if (strlen(s) >= sizeof s)
    vcl_cerr << __FILE__ ": WARNING! Possible memory corruption after call to vsprintf()\n";
  vcl_string::operator=(s);
  
  va_end(ap);
}

vcl_ostream & operator<<(vcl_ostream &os,const vbl_sprintf& s) 
{
  return os << (char const*)s; 
}

//--------------------------------------------------------------------------------

#ifdef RUNTEST
main()
{
  vcl_cout << vbl_sprintf("fred%d\n", 3);
  vcl_string fmt("foobar%d\n");
  vcl_cout << vbl_sprintf(fmt, 4);
}
#endif
