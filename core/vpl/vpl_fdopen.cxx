#include "vpl_fdopen.h"
#include <vcl_cstdio.h>

#if defined(VCL_COMO)
extern "C" FILE *fdopen(int, char const *);
#endif

vcl_FILE *vpl_fdopen(int fd, char const *mode)
{
#if defined(VCL_BORLAND)
  return _fdopen(fd, const_cast<char*>(mode));
#else
  return fdopen(fd, mode);
#endif
}
