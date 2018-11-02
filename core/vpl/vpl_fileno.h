// This is core/vpl/vpl_fileno.h
#ifndef vpl_fileno_h_
#define vpl_fileno_h_
//:
// \file

#include <cstdio>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vpl/vpl_export.h"

//: Return the file descriptor (int) of an open file stream (FILE *).
extern VPL_EXPORT int vpl_fileno(std::FILE *fp);

#endif
