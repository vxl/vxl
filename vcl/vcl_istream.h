#ifndef vcl_istream_h_
#define vcl_istream_h_

#include "vcl_compiler.h"
#include <istream>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_basic_istream std::basic_istream
#define vcl_istream std::istream
#define vcl_wistream std::wistream
#define vcl_basic_iostream std::basic_iostream
#define vcl_iostream std::iostream
#define vcl_wiostream std::wiostream

#endif // vcl_istream_h_
