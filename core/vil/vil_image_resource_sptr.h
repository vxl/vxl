// This is core/vil/vil_image_resource_sptr.h
#ifndef vil_image_resource_sptr_h_
#define vil_image_resource_sptr_h_

//:
// \file
// Smart pointer to a image resource.

#include <vil/vil_smart_ptr.h>

class vil_image_resource;

//: Use this type to refer to and store a vil_image_resource
// This object is used to provide safe manipulation of
// vil_image_resource derivatives. If you want to
// store an image resource (e.g. an image on disk, type-agnostic
// memory image), then use this type.
typedef vil_smart_ptr<vil_image_resource> vil_image_resource_sptr;


#endif // vil_image_resource_sptr_h_
