// This is mul/vimt3d/vimt3d_load.cxx

//:
// \file
// \author Ian Scott

#include "vimt3d_load.h"
#include <vil3d/vil3d_property.h>

//: Create a transform from the properties of image resource.
// The transform will be from world co-ordinates in metres to image co-ordinates (or mm if requested)
vimt3d_transform_3d vimt3d_load_transform(const vil3d_image_resource_sptr &im, bool use_millimetres, bool use_000_as_origin)
{
  float width[3] = { 1.0f, 1.0f, 1.0f };
  bool valid_width = im->get_property(vil3d_property_voxel_size, width);

  if (use_millimetres)
  {
      width[0] *= 1000;
      width[1] *= 1000;
      width[2] *= 1000;
  }

  float offset[3] = { 0.0f, 0.0f, 0.0f };
  bool valid_offset = im->get_property(vil3d_property_origin_offset, offset);

  // force to use 000 as the origin
  if (use_000_as_origin)
  {
      offset[0] = 0.0f;
      offset[1] = 0.0f;
      offset[2] = 0.0f;
  }

  vimt3d_transform_3d t;

  if (!valid_width)
    if (!valid_offset)
    {
      t.set_identity();
      return t;
    }
    else
    {
      t.set_translation(offset[0], offset[1], offset[2]);
      return t;
    }
  else
    if (!valid_offset)
    {
      t.set_zoom_only(1.0/width[0], 1.0/width[1], 1.0/width[2], 0.0f, 0.0f, 0.0f);
      return t;
    }
    else
    {
      t.set_zoom_only(1.0/width[0], 1.0/width[1], 1.0/width[2],
        offset[0], offset[1], offset[2]);
      return t;
    }
}
