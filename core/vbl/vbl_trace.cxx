/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vbl_trace.h"

#include <vcl_cstdio.h>

void vbl_trace_function(char const *file, int line) {
  fprintf(stderr, "[vbl_trace %s:%d]", file, line);
  fflush(stderr);
}
