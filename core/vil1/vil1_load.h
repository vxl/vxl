// This is vxl/vil/vil_load.h
#ifndef vil_load_h_
#define vil_load_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief read an image from a file
//
// vil_load() returns a vil_image which will "read an image from a file". In practice
// it may or may not read the image on creation, but a disk read will take place at
// some point before get_section() returns.
//
// vil_load() ensures image semantics with regard to vertical orientation:
// the first image row is always the top pixel row of the image.
// The disk-stored file could have a different convention, in which case
// row swapping is performed.
//
// To avoid this possible row swapping, use the vil_load_raw() functions.
//
// \author awf@robots.ox.ac.uk
//
// \verbatim
//  Modifications
//     011002 Peter Vanroose - vil_load now respects top-is-first; vil_load_raw not
//\endverbatim

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
