// This is core/vil/vil_load.h
#ifndef vil_load_h_
#define vil_load_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief read an image from a file
//
// vil_load returns a pointer to the base class of an vil_image_view object.
// You can then cast, or assign it to the appropriate type. You must not forget to
// delete the created view.
//
// vil_load_image_resource() returns a smart pointer to vil_image_resource which
// will "read an image from a file". In practice
// it may or may not read the image on creation, but a disk read will take place at
// some point before get_view() returns.
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
//     24 Sep 2002 Ian Scott - converted to vil
//\endverbatim

#include <vil/vil_fwd.h>
#include <vil/vil_image_resource.h>

//: Load an image data object from a file, possibly performing expected conversions.
// \relates vil_image_resource
vil_image_resource_sptr vil_load_image_resource(char const* filename);

//: Load raw from stream.
vil_image_resource_sptr vil_load_image_resource_raw(vil_stream *);

//: Load raw from a filename.
// A convenience function.
vil_image_resource_sptr vil_load_image_resource_raw(char const*);

//: Load from a filename with a plugin.
vil_image_resource_sptr vil_load_image_resource_plugin(char const*);

//: Convenience function for loading an image into an image view.
// \relates vil_image_view
vil_image_view_base_sptr vil_load(const char *);

#endif // vil_load_h_
