// This is core/vpl/vpl_fdopen.h
#ifndef vpl_fdopen_h_
#define vpl_fdopen_h_
//:
// \file

#include <vcl_cstdio.h>

//: Turn an open file descriptor (int) into a file stream (FILE *).
vcl_FILE *vpl_fdopen(int fd, char const *mode);

#endif
