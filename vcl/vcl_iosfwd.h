#ifndef vcl_iosfwd_h_
#define vcl_iosfwd_h_

#include "vcl_compiler.h"
#include <iosfwd>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
#include "vcl_sstream.h"
#include "vcl_streambuf.h"
#include "vcl_iostream.h"
#include "vcl_istream.h"
#include "vcl_ostream.h"
#include "vcl_fstream.h"
#include "vcl_ios.h"
#include "vcl_memory.h"
#include "vcl_iterator.h"
#include "vcl_string.h"
//vcl alias names to std names
#define vcl_basic_istringstream std::basic_istringstream
#define vcl_basic_ostringstream std::basic_ostringstream
#define vcl_basic_filebuf std::basic_filebuf
#define vcl_basic_ifstream std::basic_ifstream
#define vcl_basic_ofstream std::basic_ofstream
#define vcl_basic_fstream std::basic_fstream
#define vcl_istreambuf_iterator std::istreambuf_iterator
#define vcl_ostreambuf_iterator std::ostreambuf_iterator
#define vcl_wios std::wios
#define vcl_stringstream std::stringstream
#define vcl_wstreambuf std::wstreambuf
#define vcl_wistringstream std::wistringstream
#define vcl_wostringstream std::wostringstream
#define vcl_wstringstream std::wstringstream
#define vcl_wfilebuf std::wfilebuf
#define vcl_wifstream std::wifstream
#define vcl_wofstream std::wofstream
#define vcl_wfstream std::wfstream
#define vcl_wstreampos std::wstreampos

#endif // vcl_iosfwd_h_
