// This is core/vul/vul_trace.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "vul_trace.h"
#include <vcl_iostream.h>

void vul_trace_function(char const *file, int line)
{
  vcl_cerr << "[vul_trace " << file << ':' << line << ']' << vcl_flush;
}
