#include "vpl_fdopen.h"

#if defined(VCL_BORLAND_56)
# include <stdio.h> /* For _fdopen on Borland 5.6 */
#endif

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
