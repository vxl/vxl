#ifndef vcl_iso_sstream_h_
#define vcl_iso_sstream_h_

#include <sstream>

// basic_stringbuf
#ifndef vcl_basic_stringbuf
#define vcl_basic_stringbuf std::basic_stringbuf
#endif
// stringbuf
#ifndef vcl_stringbuf
#define vcl_stringbuf std::stringbuf
#endif
// wstringbuf
#ifndef vcl_wstringbuf
#define vcl_wstringbuf std::wstringbuf
#endif
// istringstream
#ifndef vcl_istringstream
#define vcl_istringstream std::istringstream
#endif
// ostringstream
#ifndef vcl_ostringstream
#define vcl_ostringstream std::ostringstream
#endif

using std::ostringstream;
using std::istringstream;

#endif // vcl_iso_sstream_h_
