#ifndef vcl_cwctype_h_
#define vcl_cwctype_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include "vcl_compiler.h"

#ifndef hpux
#include <wctype.h>
#else // HP does not have /usr/include/wctype.h
#include <wchar.h>
#endif

#endif // vcl_cwctype_h_
