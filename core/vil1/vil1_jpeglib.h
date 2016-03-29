// This is core/vil1/vil1_jpeglib.h
#ifndef vil1_jpeglib_h_
#define vil1_jpeglib_h_
//:
// \file
// \brief Stuff for jpeg I/O
// \author fsm

#include <cstdio>
#include <cstddef>
#include <vcl_compiler.h>

// ?
extern "C" {
#define boolean jpeg_boolean
#include <jpeglib.h>
#include <jerror.h>
#undef boolean
}

#endif // vil1_jpeglib_h_
