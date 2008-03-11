// This is mul/vimt/vimt_load_transform.h
#ifndef vimt_load_transform_h_
#define vimt_load_transform_h_

//:
// \file
// \author Martin Roberts

#include <vil/vil_image_resource.h>
#include <vimt/vimt_transform_2d.h>

//: Create a transform from the properties of image resource.
// \param unit_scaling is to convert from metres to desired world units (e.g. 1000 for mm)
vimt_transform_2d vimt_load_transform(const vil_image_resource_sptr &im,
                                      float unit_scaling=1.0f);

//: Create a transform from the properties of image resource, assuming a right-hand world frame.
// \param unit_scaling is to convert from metres to desired world units (e.g. 1000 for mm)
// \note This version incorporates a reflection through the x-axis so that 
// the transform is put into a right-handed coordinate frame 
// (with y increasing from bottom to top of image).
vimt_transform_2d vimt_load_transform_right_hand(const vil_image_resource_sptr &im,
                                                float unit_scaling=1.0f);

#endif // vimt_load_transform_h_
