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
// vil2_load returns a pointer to the base class of an vil2_image_view object.
// You can then cast, or assign it to the appropriate type. You must not forget to
// delete the created view.
//
// vil2_load_image_data() returns a smart pointer to vil2_image_data which
// will "read an image from a file". In practice
// it may or may not read the image on creation, but a disk read will take place at
// some point before get_view() returns.
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
//     24 Sep 2002 Ian Scott - converted to vil2
//\endverbatim

#include <vcl_iosfwd.h>
#include <vil/vil_fwd.h>
#include <vil2/vil2_image_data.h>

//: Load an image data object from a file, possibly performing expected conversions.
vil2_image_data_sptr vil2_load_image_data(char const* filename);

//: Load raw from stream.
vil2_image_data_sptr vil2_load_image_data_raw(vil_stream *);

//: Load raw from a filename.
// A convience function.
vil2_image_data_sptr vil2_load_image_data_raw(char const*);

//: Convenience function for loading an image into an image view.
vil2_image_view_base_sptr vil2_load(const char *);

#endif // vil2_load_h_
