#ifndef vil_load_h_
#define vil_load_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vil/vil_load.h

//:
// \file
// \brief read an image from a file
//
// vil_load*() returns a vil_image which will "read an image from a file". In practice
// it may or may not read the image on creation, but a disk read will take place at
// some point before get_section() returns.
//
// \author awf@robots.ox.ac.uk

#include <vcl_iosfwd.h>
#include <vil/vil_fwd.h>
#include <vil/vil_image.h>

//: Load an image from a file, possibly performing expected conversions.
vil_image vil_load(char const* filename);

//: Load raw from stream.
vil_image vil_load_raw(vil_stream *);

//: Load raw from file (convenience).
vil_image vil_load_raw(char const *);

#endif // vil_load_h_
