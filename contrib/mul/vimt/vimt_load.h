// This is mul/vimt/vimt_load.h
#ifndef vimt_load_h_
#define vimt_load_h_
//:
// \file
// \author Martin Roberts, Ian Scott

#include <iostream>
#include <cstring>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vil/vil_flatten.h>
#include <vimt/vimt_image_2d_of.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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

//: Check if an image is a DICOM image (returns true if image format can be confirmed
//  as being DICOM)
bool vimt_is_image_dicom(vil_stream* is);

//: Check for a DICOM image if tag PhotometricInterpretation is set to MONOCHROME1
bool vimt_is_monochrome1(const std::string& im_path);

//: Load image from path into byte image, merging transparent image planes
// If input image is float or int16 then stretch values to byte
void vimt_load_to_byte(const std::string& im_path, vimt_image_2d_of<vxl_byte>& image,
                       float unit_scaling);

//: Load image from path into float image, merging transparent image planes
void vimt_load_to_float(const std::string& im_path, vimt_image_2d_of<float>& image,
                       float unit_scaling);

//: Load image from path into given image (forcing to given pixel type)
// \param unit_scaling is to convert from metres to desired world units (e.g. 1000 for mm)
template<class T> inline
void vimt_load(const std::string& path,
               vimt_image_2d_of<T>& image,
               float unit_scaling=1.0f)
{
  vil_image_resource_sptr ir = vil_load_image_resource(path.c_str());
  if (ir.ptr()==nullptr)
  {
    image.image().set_size(0,0);
    return;
  }
  image.image() = vil_convert_cast(T(),ir->get_view(0,ir->ni(),0,ir->nj()));
  image.set_world2im(vimt_load_transform(ir, unit_scaling));
}


//: Load image from path into given image (forcing to given pixel type), merging transparent planes
// \param unit_scaling is to convert from metres to desired world units (e.g. 1000 for mm)
template<class T> inline
void vimt_load_as_grey_or_rgb(const std::string& path,
               vimt_image_2d_of<T>& image,
               float unit_scaling=1.0f)
{
  vil_image_resource_sptr ir = vil_load_image_resource(path.c_str());

  if (ir.ptr()==nullptr)
  {
    image.image().set_size(0,0);
    return;
  }

  image.image() = vil_convert_cast(T(),ir->get_view(0,ir->ni(),0,ir->nj()));
  image.set_world2im(vimt_load_transform(ir, unit_scaling));

  unsigned nplanes = image.image().nplanes();

  // merge transparent plane in tiff and png images
  if ((strcmp(ir->file_format(), "tiff") == 0) || (strcmp(ir->file_format(), "png") == 0))
  {
    if ((nplanes == 2) || (nplanes == 4))
    {
      vil_image_view_base_sptr image_ref = new vil_image_view<T>(image.image());
      vil_image_view<T> new_image_view = vil_convert_to_n_planes(nplanes-1, image_ref);

      vil_convert_merge_alpha(image.image(), new_image_view, nplanes);
      image.image() = vil_convert_to_n_planes(nplanes-1, image_ref);
      image.image() = new_image_view;
    }
  }
}

//: Load image from path into given image (forcing to given pixel type)
// \param unit_scaling is to convert from metres to desired world units (e.g. 1000 for mm)
// \note This version incorporates a reflection through the x-axis so that
// the transform is put into a right-handed coordinate frame
// (with y increasing from bottom to top of image).
template<class T> inline
void vimt_load_right_hand(const std::string& path,
                          vimt_image_2d_of<T>& image,
                          float unit_scaling=1.0f)
{
  vil_image_resource_sptr ir = vil_load_image_resource(path.c_str());
  if (ir.ptr()==nullptr)
  {
    image.image().set_size(0,0);
    return;
  }

  image.image() = vil_convert_cast(T(),
    ir->get_view(0,ir->ni(),0,ir->nj()));

  image.set_world2im(vimt_load_transform_right_hand(ir, unit_scaling));
}


#endif // vimt_load_h_
