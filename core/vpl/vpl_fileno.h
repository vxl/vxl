// This is core/vpl/vpl_fileno.h
#ifndef vpl_fileno_h_
#define vpl_fileno_h_
//:
// \file

#include <vcl_compiler.h>
#include <cstdio>

//: Return the file descriptor (int) of an open file stream (FILE *).
int vpl_fileno(std::FILE *fp);

#endif
