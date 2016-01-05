// This is core/vpl/vpl_fileno.cxx
#include "vpl_fileno.h"

int vpl_fileno(vcl_FILE *fp)
{
#if defined(VCL_WIN32) && !defined(__CYGWIN__) && _MSC_VER >= 1400
  return _fileno(fp);
#else
  return fileno(fp);
#endif
}
