// This is mul/vil2/file_formats/vil2_jpeglib.h
#ifndef vil2_jpeglib_h_
#define vil2_jpeglib_h_
//:
// \file
// \brief Stuff for jpeg I/O
// \author fsm@robots.ox.ac.uk

#include <vcl_cstdio.h> // for FILE, used in jpeglib.h !!
#include <vcl_cstddef.h> // for size_t, used in jpeglib.h !!

// ?
extern "C" {
#define boolean jpeg_boolean
#include <jpeglib.h>
#include <jerror.h>
#undef boolean
}

#endif // vil2_jpeglib_h_
