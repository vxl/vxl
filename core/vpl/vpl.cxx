#include "vpl.h"

#if defined(__CYGWIN__)
# include "os_unix/vpl.cxx"
#elif defined(_MSC_VER)
# include "os_win32/vpl.cxx"
#else
# include "os_unix/vpl.cxx"
#endif
