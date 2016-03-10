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
//vcl alias names to std names
#define vcl_streamoff std::streamoff
#define vcl_char_traits std::char_traits
#define vcl_allocator std::allocator
#define vcl_basic_ios std::basic_ios
#define vcl_basic_streambuf std::basic_streambuf
#define vcl_basic_istream std::basic_istream
#define vcl_basic_ostream std::basic_ostream
#define vcl_basic_iostream std::basic_iostream
#define vcl_basic_istringstream std::basic_istringstream
#define vcl_basic_ostringstream std::basic_ostringstream
#define vcl_basic_filebuf std::basic_filebuf
#define vcl_basic_ifstream std::basic_ifstream
#define vcl_basic_ofstream std::basic_ofstream
#define vcl_basic_fstream std::basic_fstream
#define vcl_istreambuf_iterator std::istreambuf_iterator
#define vcl_ostreambuf_iterator std::ostreambuf_iterator
#define vcl_ios std::ios
#define vcl_wios std::wios
#define vcl_streambuf std::streambuf
#define vcl_istream std::istream
#define vcl_ostream std::ostream
#define vcl_iostream std::iostream
#define vcl_stringstream std::stringstream
#define vcl_filebuf std::filebuf
#define vcl_ifstream std::ifstream
#define vcl_ofstream std::ofstream
#define vcl_fstream std::fstream
#define vcl_wstreambuf std::wstreambuf
#define vcl_wistream std::wistream
#define vcl_wostream std::wostream
#define vcl_wiostream std::wiostream
#define vcl_wstringbuf std::wstringbuf
#define vcl_wistringstream std::wistringstream
#define vcl_wostringstream std::wostringstream
#define vcl_wstringstream std::wstringstream
#define vcl_wfilebuf std::wfilebuf
#define vcl_wifstream std::wifstream
#define vcl_wofstream std::wofstream
#define vcl_wfstream std::wfstream
#define vcl_fpos std::fpos
#define vcl_streampos std::streampos
#define vcl_wstreampos std::wstreampos

#endif // vcl_iosfwd_h_
