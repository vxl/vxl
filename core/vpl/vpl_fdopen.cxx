// This is core/vpl/vpl_fdopen.cxx
#include "vpl_fdopen.h"
#include <vcl_cstdio.h>

vcl_FILE *vpl_fdopen(int fd, char const *mode)
{
#if defined(VCL_BORLAND_55)
  return fdopen(fd, const_cast<char*>(mode));
#elif defined(VCL_BORLAND_56)
  return std::fdopen(fd, const_cast<char*>(mode));
#elif defined(VCL_WIN32) && !defined(__CYGWIN__) && _MSC_VER >= 1400
  return _fdopen(fd, mode);
#else
  return fdopen(fd, mode);
#endif
}
