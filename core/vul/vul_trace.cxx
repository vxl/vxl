// This is core/vul/vul_trace.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include <iostream>
#include "vul_trace.h"
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void vul_trace_function(char const *file, int line)
{
  std::cerr << "[vul_trace " << file << ':' << line << ']' << std::flush;
}
