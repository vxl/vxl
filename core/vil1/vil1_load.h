// This is core/vil1/vil1_load.h
#ifndef vil1_load_h_
#define vil1_load_h_
//:
// \file
// \brief read an image from a file
//
// vil1_load() returns a vil1_image which will "read an image from a file". In practice
// it may or may not read the image on creation, but a disk read will take place at
// some point before get_section() returns.
//
// vil1_load() ensures image semantics with regard to vertical orientation:
// the first image row is always the top pixel row of the image.
// The disk-stored file could have a different convention, in which case
// row swapping is performed.
//
// To avoid this possible row swapping, use the vil1_load_raw() functions.
//
// \author awf@robots.ox.ac.uk
//
// \verbatim
//  Modifications
//     011002 Peter Vanroose - vil1_load now respects top-is-first; vil1_load_raw not
//\endverbatim

#include <vil1/vil1_fwd.h>
#include <vil1/vil1_image.h>

//: Load an image from a file, possibly performing expected conversions.
vil1_image vil1_load(char const* filename);

//: Load raw from stream.
vil1_image vil1_load_raw(vil1_stream *);

//: Load raw from file (convenience).
vil1_image vil1_load_raw(char const *);

#endif // vil1_load_h_
