// This is core/vpl/vpl_fdopen.h
#ifndef vpl_fdopen_h_
#define vpl_fdopen_h_
//:
// \file

#include <cstdio>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vpl/vpl_export.h"

//: Turn an open file descriptor (int) into a file stream (FILE *).
extern VPL_EXPORT std::FILE *vpl_fdopen(int fd, char const *mode);

#endif
