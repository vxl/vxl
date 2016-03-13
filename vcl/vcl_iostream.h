#ifndef vcl_iostream_h_
#define vcl_iostream_h_

#include "vcl_compiler.h"
#include <iostream>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
#include "vcl_iosfwd.h"
#include "vcl_cstddef.h"
#include "vcl_ios.h"
#include "vcl_iomanip.h"
#include "vcl_istream.h"
#include "vcl_ostream.h"
#include "vcl_streambuf.h"
#include "vcl_iomanip.h"
//vcl alias names to std names
#define vcl_cin std::cin
#define vcl_cout std::cout
#define vcl_cerr std::cerr
#define vcl_clog std::clog
#define vcl_wcout std::wcout
// the following are not actually in the
// <iostream> synopsis. for example, endl
// and ends actually come from <ostream>.
// hex, oct, dec, ws, setprecision, setw,
// setfill, fixed, scientific come from
// <iomanip>.
#define vcl_ios_in std::ios::in
#define vcl_ios_out std::ios::out
#define vcl_ios_ate std::ios::ate
#define vcl_ios_app std::ios::app
#define vcl_ios_trunc std::ios::trunc
#define vcl_ios_binary std::ios::binary
#define vcl_ios_openmode std::ios::openmode
#define vcl_streampos std::streampos
#define vcl_ws std::ws

#endif // vcl_iostream_h_
