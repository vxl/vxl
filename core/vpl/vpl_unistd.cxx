#include "vpl_unistd.h"

#if defined(__CYGWIN__)
# include <vpl/os_cygwin/vpl_unistd.cxx>
#elif defined(_MSC_VER)
# include <vpl/os_win32/vpl_unistd.cxx>
#else
# include <vpl/os_unix/vpl_unistd.cxx>
#endif
