#ifndef vcl_cwctype_h_
#define vcl_cwctype_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include "vcl_compiler.h"

#ifdef VCL_SGI_CC_720
# include <wctype.h>
#elif defined(hpux)
// PVr: HP does not have /usr/include/wctype.h
// fsm: but the compiler might supply <cwctype>?
// PVr: but that file probably just #includes wctype.h?
# include <wchar.h>
#else
# include "iso/vcl_cwctype.h"
#endif

#endif // vcl_cwctype_h_
