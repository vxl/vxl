#include <vpl/vpl.h>

#if defined(__CYGWIN__)
# include <vpl/os_unix/vpl.cxx>
#elif defined(_MSC_VER)
# include <vpl/os_win32/vpl.cxx>
#else
# include <vpl/os_unix/vpl.cxx>
#endif
