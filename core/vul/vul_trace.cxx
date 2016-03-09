// This is core/vul/vul_trace.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "vul_trace.h"
#include <vcl_compiler.h>
#include <iostream>

void vul_trace_function(char const *file, int line)
{
  std::cerr << "[vul_trace " << file << ':' << line << ']' << std::flush;
}
