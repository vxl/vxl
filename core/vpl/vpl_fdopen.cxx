#include "vpl_fdopen.h"
#include <vcl_cstdio.h>

#if defined(VCL_COMO)
extern "C" FILE *fdopen(int, char const *);
#endif

vcl_FILE *vpl_fdopen(int fd, char const *mode)
{
#if defined(VCL_BORLAND_55)
  return fdopen(fd, const_cast<char*>(mode));
#elif defined(VCL_BORLAND_56)
  return std::fdopen(fd, const_cast<char*>(mode));
#else
  return fdopen(fd, mode);
#endif
}
