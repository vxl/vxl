// This is core/vpl/vpl_fileno.cxx
#include "vpl_fileno.h"

#if defined(VCL_COMO)
extern "C" int fileno(FILE *);
#endif

int vpl_fileno(vcl_FILE *fp)
{
  return fileno(fp);
}
