#include "vpl_fdopen.h"

#if defined(VCL_COMO)
extern "C" FILE *fdopen(int, char const *);
#endif

vcl_FILE *vpl_fdopen(int fd, char const *mode)
{
  return fdopen(fd, mode);
}
