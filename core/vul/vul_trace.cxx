// This is vxl/vul/vul_trace.cxx

//
//  fsm
//

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "vul_trace.h"

#include <vcl_cstdio.h>

void vul_trace_function(char const *file, int line) {
  vcl_fprintf(stderr, "[vul_trace %s:%d]", file, line);
  vcl_fflush(stderr);
}
