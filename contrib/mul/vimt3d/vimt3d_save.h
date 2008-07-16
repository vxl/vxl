// This is mul/vimt3d/vimt3d_save.cxx

//:
// \file
// \author Ian Scott

#include <vil3d/vil3d_image_resource.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_convert.h>

//: Save values from a transform to an image resource.
// The transform will be from world co-ordinates in metres to image co-ordinates (or mm if requested).
void vimt3d_save_transform(vil3d_image_resource_sptr &ir,
  const vimt3d_transform_3d& trans, bool use_millimetres =false);



//: Save image from path into given image (forcing to given pixel type)
template<class T>
void vimt3d_save(const vcl_string& path,
                 vimt3d_image_3d_of<T>& image,
                 bool use_millimetres =false)
{
  vil3d_image_resource_sptr ir = vil3d_load_image_resource(path.c_str());
  if (ir.ptr()==0)
  {
    image.image().set_size(0,0,0);
    return;
  }

  image.image() = vil3d_convert_cast(T(), 
                                     ir->get_view(0,ir->ni(),0,ir->nj(),0,ir->nk()));

  bool use_millimeters=true;
  image.set_world2im(vimt3d_load_transform(ir,use_millimeters));
}
