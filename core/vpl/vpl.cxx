// This is core/vpl/vpl.cxx
#include "vpl.h"

#if defined(__CYGWIN__)
# include "os_unix/vpl.cxx"
#elif defined(VCL_WIN32)
# include "os_win32/vpl.cxx"
#else
# include "os_unix/vpl.cxx"
#endif
