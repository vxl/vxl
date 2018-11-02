// This is core/vil/file_formats/vil_jpeglib.h
#ifndef vil_jpeglib_h_
#define vil_jpeglib_h_
//:
// \file
// \brief Stuff for jpeg I/O
// \author fsm

#include <cstdio>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// ?
extern "C" {
#define boolean jpeg_boolean
#include <jpeglib.h>
#include <jerror.h>
#undef boolean
}

#endif // vil_jpeglib_h_
