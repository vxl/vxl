// This is core/vil1/vil1_jpeglib.h
#ifndef vil1_jpeglib_h_
#define vil1_jpeglib_h_
//:
// \file
// \brief Stuff for jpeg I/O
// \author fsm

#include <vcl_cstdio.h> // for FILE, used in jpeglib.h !!
#include <vcl_cstddef.h> // for size_t, used in jpeglib.h !!

// ?
extern "C" {
#define boolean jpeg_boolean
#include <jpeglib.h>
#include <jerror.h>
#undef boolean
}

#endif // vil1_jpeglib_h_
