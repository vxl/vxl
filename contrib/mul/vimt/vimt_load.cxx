// This is mul/vimt/vimt_load.cxx

//:
// \file
// \author Martin Roberts

#include "vimt_load.h"
#include <vil/vil_property.h>
#include "vimt_convert.h"


// Create a transform from the properties of image resource.
// NB unit scaling is to convert from metres to desired world unts (e.g. 1000.0 for mm)
vimt_transform_2d vimt_load_transform(const vil_image_resource_sptr& im,
                                      float unit_scaling)
{
  float pixSize[2] = { 1.0f, 1.0f };
  bool valid_pixSize = im->get_property(vil_property_pixel_size, pixSize);

  float offset[2] = { 0.0f, 0.0f};
  bool valid_offset = im->get_property(vil_property_offset, offset);
  if (!valid_offset)
  {
    offset[0]=offset[1]=0.0f;
  }

  for(unsigned i=0; i<2;i++)
  {
    pixSize[i]*= unit_scaling;
  }

  vimt_transform_2d tx;
  if (valid_pixSize)
  {
    tx.set_zoom_only(1.0/pixSize[0], 1.0/pixSize[1],
                     offset[0], offset[1]); //NB offsets will be zero if offset prop invalid
  }
  else if( valid_offset)
  {
    tx.set_translation(offset[0], offset[1]);
  }
  else
  {
    //Both invalid
    tx.set_identity();
  }

  return tx;
}

// Create a transform from the properties of image resource, assuming a right-hand world frame.
// NB unit scaling is to convert from metres to desired world unts (e.g. 1000.0 for mm)
// This version incorporates a reflection through the x-axis so that
// the transform is put into a right-handed coordinate frame
// (with y increasing from bottom to top of image).
vimt_transform_2d vimt_load_transform_right_hand(const vil_image_resource_sptr& im,
                                                 float unit_scaling)
{
  float pixSize[2] = { 1.0f, 1.0f };
  bool valid_pixSize = im->get_property(vil_property_pixel_size, pixSize);
  if (!valid_pixSize)
  {
    pixSize[0] = 1.0f;
    pixSize[1] = 1.0f;
  }

  float offset[2] = { 0.0f, 0.0f};
  bool valid_offset = im->get_property(vil_property_offset, offset);
  if (!valid_offset)
  {
    offset[0] = offset[1] = 0.0f;
  }

  for(unsigned i=0; i<2; i++)
  {
    pixSize[i] *= unit_scaling;
  }

  // Include a reflection through the x-axis.
  vimt_transform_2d tx;
  tx.set_zoom_only(1.0/pixSize[0], -1.0/pixSize[1],
                   offset[0], -offset[1]);

  return tx;
}

//: Load image from path into byte image, merging transparent image planes
// If input image is float then stretch values to byte
void vimt_load_to_byte(const vcl_string& im_path, vimt_image_2d_of<vxl_byte>& image,
                       float unit_scaling)
{
  vil_image_resource_sptr ir = vil_load_image_resource(im_path.c_str());

  if (ir.ptr()==VXL_NULLPTR)
  {
    image.image().set_size(0,0);
    return;
  }

  if (ir->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    vimt_load_as_grey_or_rgb(im_path.c_str(), image, unit_scaling);
  }
  else
  if ((ir->pixel_format()==VIL_PIXEL_FORMAT_FLOAT) ||
     (ir->pixel_format()==VIL_PIXEL_FORMAT_UINT_16) ||
     (ir->pixel_format()==VIL_PIXEL_FORMAT_INT_16))
  {
    vimt_image_2d_of<float> float_image;
    vimt_load_as_grey_or_rgb(im_path.c_str(),float_image, unit_scaling);
    vimt_convert_stretch_range(float_image, image);
  }
  else
  {
    vcl_cerr<<"Unknown image pixel format ("<<ir->pixel_format()<<") for image "<<im_path.c_str()<<vcl_endl;
    vcl_abort();
  }
}


