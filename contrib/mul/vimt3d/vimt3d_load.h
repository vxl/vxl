// This is mul/vimt3d/vimt3d_load.h
#ifndef vimt3d_load_h_
#define vimt3d_load_h_
//:
// \file
// \author Ian Scott

#include <vil3d/vil3d_image_resource.h>
#include <vimt3d/vimt3d_transform_3d.h>

//: Create a transform from the properties of image resource.
// The following parameters allow the use of this method in legacy code:
// \param use_millimetres    if true load transform in millimetres (uses metres by default)
// \param use_000_as_origin  if true, uses image()(0,0,0) as origin, default is
//                           to use the origin as specified by the image format
vimt3d_transform_3d vimt3d_load_transform(const vil3d_image_resource_sptr &im,
                                          bool use_millimetres=false,
                                          bool use_000_as_origin=false);

#endif // vimt3d_load_h_
