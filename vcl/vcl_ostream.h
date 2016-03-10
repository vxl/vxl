#ifndef vcl_ostream_h_
#define vcl_ostream_h_

#include "vcl_compiler.h"
#include <ostream>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_basic_ostream std::basic_ostream
#define vcl_ostream std::ostream
#define vcl_wostream std::wostream
#define vcl_endl std::endl
#define vcl_ends std::ends
#define vcl_flush std::flush

#endif // vcl_ostream_h_
