// This is core/vpl/vpl_fdopen.cxx
#include <cstdio>
#include "vpl_fdopen.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

std::FILE *vpl_fdopen(int fd, char const *mode)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  return _fdopen(fd, mode);
#else
  return fdopen(fd, mode);
#endif
}
