// This is mul/vimt/vimt_load_transform.h
#ifndef vimt_load_transform_h_
#define vimt_load_transform_h_

//:
// \file
// \author Martin Roberts

#include <vil/vil_image_resource.h>
#include <vimt/vimt_transform_2d.h>

//: Create a transform from the properties of image resource.
//NB unit scaling is to convert from metres to desired world unts (e.g. 0.001 for mm)
vimt_transform_2d vimt_load_transform(const vil_image_resource_sptr &im,
                                                float unit_scaling=1.0f);

#endif // vimt_load_transform_h_
