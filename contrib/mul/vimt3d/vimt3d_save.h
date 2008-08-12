// This is mul/vimt3d/vimt3d_save.h
#ifndef vimt3d_save_h_
#define vimt3d_save_h_
//:
// \file
// \author Ian Scott


#include <vcl_string.h>
#include <vil3d/vil3d_fwd.h>
class vimt3d_transform_3d;
class vimt3d_image_3d;

//: Save values from a transform to an image resource.
// The transform will be from world co-ordinates in metres to image co-ordinates (or mm if requested).
void vimt3d_save_transform(vil3d_image_resource_sptr &ir,
                           const vimt3d_transform_3d& trans,
                           bool use_millimetres =false);

//: Save image from path into given image (forcing to given pixel type)
bool vimt3d_save(const vcl_string& path,
                 const vimt3d_image_3d& image,
                 bool use_millimetres =false);

#endif // vimt3d_save_h_

