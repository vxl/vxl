//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// Class: vbl_sprintf
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 08 Aug 96
// Modifications:
// 10 June 1999 fsm@robots removed constructor from 'const vcl_string &' and
//              changed remaining constructors to use do_vbl_sprintf().
//
//-----------------------------------------------------------------------------

#include "vbl_sprintf.h"

#include <vcl/vcl_cstdarg.h>
#include <vcl/vcl_cstdio.h>
#include <vcl/vcl_iomanip.h>

#include <vcl/vcl_string.h>
#include <vcl/vcl_iostream.h>

const unsigned vbl_sprintf_BUFSIZE=16384;

static void do_vbl_sprintf(vcl_string &str,const char *fmt,va_list ap)
{
  char s[vbl_sprintf_BUFSIZE];
  vsprintf(s, fmt, ap);
  if (strlen(s) >= vbl_sprintf_BUFSIZE)
    cerr << "WARNING : possible memory corruption in do_vbl_sprintf(\"" << fmt << "\",...)!\n";
  str = s;
}

vbl_sprintf::vbl_sprintf(const char *fmt, ...) : super("")
{
  va_list ap;
  va_start(ap, fmt);
  do_vbl_sprintf(*this,fmt        ,ap);
  va_end(ap);
}

#if 0
// fsm@robots. va_start(ap, fmt); gives a warning on solaris egcs 1.1.1
// It plain doesn't work for irix6 'CC -n32' as ap ends up
// as sizeof(vcl_string) plus the address of temporary
// which lives in the caller's stack frame.
vbl_sprintf::vbl_sprintf(vcl_string fmt, ...) : vcl_string("")
{
  va_list ap;
  do_vbl_sprintf(*this,fmt.c_str(),ap);
  va_end(ap);
}
#endif

vbl_sprintf::~vbl_sprintf()
{
}

vbl_sprintf::operator const char* () const 
{
  return c_str(); 
}

ostream & operator<<(ostream &os,const vbl_sprintf& s) 
{
  return os << (const char*)s; 
}

//--------------------------------------------------------------------------------

#ifdef RUNTEST
main()
{
  cout << vbl_sprintf("fred%d\n", 3);
  vcl_string fmt("foobar%d\n");
  cout << vbl_sprintf(fmt,4);
}
#endif
