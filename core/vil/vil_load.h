// This is vxl/vil2/vil2_load.h
#ifndef vil2_load_h_
#define vil2_load_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief read an image from a file
//
// vil2_load() returns a smart pointer to vil2_image_data which will "read an image from a file". In practice
// it may or may not read the image on creation, but a disk read will take place at
// some point before get_section() returns.
//
// vil2_load() ensures image semantics with regard to vertical orientation:
// the first image row is always the top pixel row of the image.
// The disk-stored file could have a different convention, in which case
// row swapping is performed.
//
// To avoid this possible row swapping, use the vil2_load_raw() functions.
//
// \author awf@robots.ox.ac.uk
//
// \verbatim
//  Modifications
//     011002 Peter Vanroose - vil2_load now respects top-is-first; vil2_load_raw not
//\endverbatim

#include <vcl_iosfwd.h>
#include <vil/vil_fwd.h>
#include <vil2/vil2_image_data.h>

//: Load an image from a file, possibly performing expected conversions.
vil2_image_data_sptr vil2_load(char const* filename);

//: Load raw from stream.
vil2_image_data_sptr vil2_load_raw(vil_stream *);

//: Load raw from a filename.
// A convience function.
vil2_image_data_sptr vil2_load_raw(char const*);


#endif // vil2_load_h_
