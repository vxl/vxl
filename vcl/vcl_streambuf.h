#ifndef vcl_streambuf_h_
#define vcl_streambuf_h_

#include "vcl_compiler.h"
#include <streambuf>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_basic_streambuf std::basic_streambuf
#define vcl_streambuf std::streambuf

#endif // vcl_streambuf_h_
