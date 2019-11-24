// This is core/vil/vil_pyramid_image_resource_sptr.h
#ifndef vil_pyramid_image_resource_sptr_h_
#define vil_pyramid_image_resource_sptr_h_

//:
// \file
// \brief Smart pointer to an image resource.

#include "vil_smart_ptr.h"

class vil_pyramid_image_resource;

//: Use this type to refer to and store a vil_pyramid_image_resource
// This object is used to provide safe manipulation of
// vil_pyramid_image_resource derivatives. If you want to
// store an image resource (e.g. an image on disk, type-agnostic
// memory image), then use this type.
typedef vil_smart_ptr<vil_pyramid_image_resource> vil_pyramid_image_resource_sptr;


#endif // vil_pyramid_image_resource_sptr_h_
