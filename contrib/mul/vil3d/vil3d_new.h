// This is mul/vil3d/vil3d_new.h
#ifndef vil3d_new_h_
#define vil3d_new_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Make a new image.

#include <vil2/vil2_fwd.h> // for vil2_stream
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_image_view.h>

//: Make a new image of given format.
// \relates vil3d_image_resource
vil3d_image_resource_sptr vil3d_new_image_resource(unsigned ni, unsigned nj, unsigned nk,
                                                  unsigned nplanes, vil2_pixel_format format);


//: Make a new image on disk.
// \relates vil3d_image_resource
vil3d_image_resource_sptr vil3d_new_image_resource(const char* filename, unsigned ni, unsigned nj,
                                                   unsigned nk, unsigned nplanes,
                                                   vil2_pixel_format format,
                                                   char const* file_format = 0);

#endif // vil3d_new_h_
