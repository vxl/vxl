// This is core/vpl/vpl_fileno.h
#ifndef vpl_fileno_h_
#define vpl_fileno_h_
//:
// \file

#include <vcl_cstdio.h>

//: Return the file descriptor (int) of an open file stream (FILE *).
int vpl_fileno(vcl_FILE *fp);

#endif
