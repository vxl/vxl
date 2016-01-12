//-*- c++ -*-------------------------------------------------------------------
#ifndef vcl_iosfwd_h_
#define vcl_iosfwd_h_

// ANSI standard iostream forward decls.
// You can't write "class ostream" and expect it to work

#include "vcl_compiler.h"

#if defined(VCL_SGI_CC_720)
# include <stream.h> // this #includes iostream.h, fstream.h and iomanip.h
# define vcl_generic_iosfwd_STD /* */
# include "generic/vcl_iosfwd.h"

#else // -------------------- iso
# include "iso/vcl_iosfwd.h"
#endif

#endif // vcl_iosfwd_h_
