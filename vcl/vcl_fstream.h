#ifndef vcl_fstream_h_
#define vcl_fstream_h_

#include "vcl_compiler.h"
#include <fstream>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
#include "vcl_iostream.h"

//vcl alias names to std names
#define vcl_filebuf std::filebuf
#define vcl_fstream std::fstream
#define vcl_ifstream std::ifstream
#define vcl_ofstream std::ofstream

#endif // vcl_fstream_h_
