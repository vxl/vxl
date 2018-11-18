// This is mul/vimt3d/vimt3d_save.h
#ifndef vimt3d_save_h_
#define vimt3d_save_h_
//:
// \file
// \author Ian Scott


#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/vil3d_fwd.h>
class vimt3d_transform_3d;
class vimt3d_image_3d;

//: Save values from a transform to an image resource.
// The transform will be from world co-ordinates in metres to image co-ordinates (or mm if requested).
void vimt3d_save_transform(vil3d_image_resource_sptr &ir,
                           const vimt3d_transform_3d& trans,
                           bool use_millimetres =false);

//: Save image + transform to given path
//  Note that since this uses the vil3d_image_resource I/O scheme, it can only guarantee that the
//  voxel size component of the world-to-image transform will be stored in full.   This is because
//  some image formats only record the voxel size, not the origin or any rotations - the interface
//  reverts to the lowest common set.  Those formats which can cope with translations will include
//  this as well.  It was felt that it is safest to be consistent and never save rotations/shears,
//  rather than have them arbitrarily saved/not saved depending on the image format chosen.
bool vimt3d_save(const std::string& path,
                 const vimt3d_image_3d& image,
                 bool use_millimetres =false);

#endif // vimt3d_save_h_
