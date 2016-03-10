#ifndef vcl_sstream_h_
#define vcl_sstream_h_

#include "vcl_compiler.h"
#include <sstream>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
#include "vcl_iosfwd.h"
//vcl alias names to std names
#define vcl_basic_stringbuf std::basic_stringbuf
#define vcl_stringbuf std::stringbuf
#define vcl_wstringbuf std::wstringbuf
#define vcl_stringstream std::stringstream
#define vcl_istringstream std::istringstream
#define vcl_ostringstream std::ostringstream

#endif // vcl_sstream_h_
