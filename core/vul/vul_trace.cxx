// This is core/vul/vul_trace.cxx
#include <iostream>
#include "vul_trace.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void vul_trace_function(char const *file, int line)
{
  std::cerr << "[vul_trace " << file << ':' << line << ']' << std::flush;
}
