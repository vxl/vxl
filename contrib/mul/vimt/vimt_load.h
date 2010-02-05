// This is mul/vimt/vimt_load.h
#ifndef vimt_load_h_
#define vimt_load_h_
//:
// \file
// \author Martin Roberts, Ian Scott

#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vimt/vimt_image_2d_of.h>

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


//: Load image from path into given image (forcing to given pixel type)
// \param unit_scaling is to convert from metres to desired world units (e.g. 1000 for mm)
template<class T> inline
void vimt_load(const vcl_string& path,
               vimt_image_2d_of<T>& image,
               float unit_scaling=1.0f)
{
  vil_image_resource_sptr ir = vil_load_image_resource(path.c_str());
  if (ir.ptr()==0)
  {
    image.image().set_size(0,0);
    return;
  }
  image.image() = vil_convert_cast(T(),ir->get_view(0,ir->ni(),0,ir->nj()));
  image.set_world2im(vimt_load_transform(ir, unit_scaling));
}


//: Load image from path into given image (forcing to given pixel type)
// \param unit_scaling is to convert from metres to desired world units (e.g. 1000 for mm)
// \note This version incorporates a reflection through the x-axis so that
// the transform is put into a right-handed coordinate frame
// (with y increasing from bottom to top of image).
template<class T> inline
void vimt_load_right_hand(const vcl_string& path,
                          vimt_image_2d_of<T>& image,
                          float unit_scaling=1.0f)
{
  vil_image_resource_sptr ir = vil_load_image_resource(path.c_str());
  if (ir.ptr()==0)
  {
    image.image().set_size(0,0);
    return;
  }

  image.image() = vil_convert_cast(T(),
    ir->get_view(0,ir->ni(),0,ir->nj()));

  image.set_world2im(vimt_load_transform_right_hand(ir, unit_scaling));
}


#endif // vimt_load_h_
