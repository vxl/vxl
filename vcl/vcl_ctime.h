#ifndef vcl_ctime_h_
#define vcl_ctime_h_

// ??  This is a customization that is not part of the
// macro used to auto generate this header file
#ifndef VCL_WIN32
#include <sys/times.h>
#endif

#include "vcl_compiler.h"
#include <ctime>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
// NB: size_t is declared in <cstddef>, not <ctime>
#define vcl_clock_t std::clock_t
#define vcl_time_t std::time_t
#define vcl_tm std::tm
#define vcl_asctime std::asctime
#define vcl_clock std::clock
#define vcl_difftime std::difftime
#define vcl_localtime std::localtime
#define vcl_strftime std::strftime
#define vcl_ctime std::ctime
#define vcl_gmtime std::gmtime
#define vcl_mktime std::mktime
#define vcl_time std::time

#endif // vcl_ctime_h_
