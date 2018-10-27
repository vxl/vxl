// This is mul/vimt3d/vimt3d_load.cxx
#include "vimt3d_load.h"
//:
// \file
// \author Ian Scott, Tim Cootes

#include <vil3d/vil3d_property.h>
#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_convert.h>

//: Create a transform from the properties of image resource.
// The transform will be from world co-ordinates in metres to image co-ordinates (or mm if requested).
vimt3d_transform_3d vimt3d_load_transform(const vil3d_image_resource_sptr &im, bool use_millimetres)
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

//: Load 3D image and return pointer.  Caller responsible for deleting object
//  Object will be of type matching pixel format in file, such as
//  vimt3d_image_3d_of<vxl_byte>  or vimt3d_image_3d_of<float>
vimt3d_image_3d* vimt3d_load(const std::string& path, bool use_mm)
{
  vil3d_image_resource_sptr ir = vil3d_load_image_resource(path.c_str());
  if (ir.ptr()==nullptr)
  {
    return nullptr;
  }

  if (ir->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    auto* image = new vimt3d_image_3d_of<vxl_byte>();
    image->image() = vil3d_convert_cast(vxl_byte(),
                                      ir->get_view(0,ir->ni(),0,ir->nj(),0,ir->nk()));

    image->set_world2im(vimt3d_load_transform(ir,use_mm));
    return image;
  }
  if (ir->pixel_format()==VIL_PIXEL_FORMAT_INT_16)
  {
    auto* image = new vimt3d_image_3d_of<vxl_int_16>();
    image->image() = vil3d_convert_cast(vxl_int_16(),
                                        ir->get_view(0,ir->ni(),0,ir->nj(),0,ir->nk()));

    image->set_world2im(vimt3d_load_transform(ir,use_mm));
    return image;
  }
  if (ir->pixel_format()==VIL_PIXEL_FORMAT_INT_32)
  {
    auto* image = new vimt3d_image_3d_of<vxl_int_32>();
    image->image() = vil3d_convert_cast(vxl_int_32(),
                                        ir->get_view(0,ir->ni(),0,ir->nj(),0,ir->nk()));

    image->set_world2im(vimt3d_load_transform(ir,use_mm));
    return image;
  }
  if (ir->pixel_format()==VIL_PIXEL_FORMAT_FLOAT)
  {
    auto* image = new vimt3d_image_3d_of<float>();
    image->image() = vil3d_convert_cast(float(),
                                      ir->get_view(0,ir->ni(),0,ir->nj(),0,ir->nk()));

    image->set_world2im(vimt3d_load_transform(ir,use_mm));
    return image;
  }

  std::cerr<<"vimt3d_load() Unknown pixel format: "<<ir->pixel_format()<<std::endl;
  return nullptr;
}
