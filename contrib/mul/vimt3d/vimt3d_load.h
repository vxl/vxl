// This is mul/vimt3d/vimt3d_load.h
#ifndef vimt3d_load_h_
#define vimt3d_load_h_
//:
// \file
// \author Ian Scott

#include <vil3d/vil3d_image_resource.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_convert.h>

//: Create a transform from the properties of image resource.
// The following parameters allow the use of this method in legacy code:
// \param use_millimetres    if true load transform in millimetres (uses metres by default)
vimt3d_transform_3d vimt3d_load_transform(const vil3d_image_resource_sptr &im,
                                          bool use_millimetres=false);

//: Load image from path into given image (forcing to given pixel type)
template<class T>
void vimt3d_load(const vcl_string& path,
                 vimt3d_image_3d_of<T>& image)
{
  vil3d_image_resource_sptr ir = vil3d_load_image_resource(path.c_str());
  if (ir.ptr()==0)
  {
    image.image().set_size(0,0,0);
    return;
  }

  image.image() = vil3d_convert_cast(T(), 
                                     ir->get_view(0,ir->ni(),0,ir->nj(),0,ir->nk()));
  if (image.image().size()==0) return;
  bool use_millimeters=true;
  image.set_world2im(vimt3d_load_transform(ir,use_millimeters));
}

#endif // vimt3d_load_h_
