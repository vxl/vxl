// This is brl/bpro/core/vil_pro/processes/vil_convert_pixel_type_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Converts floating point image (0,1.0) to byte image (0,255) for compression/saveability reasons

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view_base.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>

//: Constructor
bool vil_convert_pixel_type_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs:
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("vcl_string");

  //this process has 1 output
  // output(0): the output image with the specified number of planes
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // label image

  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vil_convert_pixel_type_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()<2) {
    std::cout << "vil_convert_pixel_type_process: The input number should be 2" << std::endl;
    return false;
  }
  unsigned i=0;

  //Retrieve image from input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);

  //retrieve output type
  std::string out_type = pro.get_input<std::string>(i++);

  ////////////////////////////////////////////////////////////////////
  //Convert to float image
  ////////////////////////////////////////////////////////////////////
  vil_image_view<float> fimage;
  if (img->pixel_format() == VIL_PIXEL_FORMAT_RGBA_BYTE ) {
    vil_image_view_base_sptr plane_image = vil_convert_to_n_planes(4, img);
    fimage = *vil_convert_cast(float(), plane_image);
    vil_math_scale_values(fimage, 1.0/255.0);
  }
  else {
    //can use convert cast
    fimage = *vil_convert_cast(float(), img);

    // convert input image to float in range from 0 to 1
    if (img->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
      vil_math_scale_values(fimage,1.0/255.0);
    if (img->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
      vil_math_scale_values(fimage,1.0/4096);
  }
  unsigned ni = fimage.ni();
  unsigned nj = fimage.nj();
  unsigned nplanes = fimage.nplanes();

  ////////////////////////////////////////////////////////////////////
  //return image of same type (convert back)
  ////////////////////////////////////////////////////////////////////
  if (out_type=="float") {
    auto* bimage = new vil_image_view<float>(ni, nj, nplanes);
    vil_convert_cast(fimage, *bimage);
    pro.set_output_val<vil_image_view_base_sptr>(0, bimage);
    return true;
  }
  else if (out_type=="byte") {
    vil_math_scale_values( fimage, 255.0 );
    auto* bimage = new vil_image_view<vxl_byte>(ni, nj, nplanes);
    vil_convert_cast(fimage, *bimage);
    pro.set_output_val<vil_image_view_base_sptr>(0, bimage);
    return true;
  }
  else if (out_type=="uint16") {
    vil_math_scale_values( fimage, 65535.0 );
    auto* bimage = new vil_image_view<vxl_uint_16>(ni, nj, nplanes);
    vil_convert_cast(fimage, *bimage);
    pro.set_output_val<vil_image_view_base_sptr>(0, bimage);
    return true;
  }
  else if (out_type=="rgba")    {
    vil_math_scale_values( fimage, 255.0);
    auto* bimage = new vil_image_view<vxl_byte>(ni, nj, nplanes);
    vil_convert_cast(fimage, *bimage);
    pro.set_output_val<vil_image_view_base_sptr>(0, vil_convert_to_component_order(bimage));
    return true;
  }
  else if (out_type=="grey")   {
    auto* floatimg = new vil_image_view<float>(img->ni(), img->nj());
    if (img->nplanes() == 3 || img->nplanes() == 4) {
      auto* inImg = dynamic_cast<vil_image_view<vxl_byte>* >(img.ptr());
      vil_image_view<float>     greyImg(img->ni(), img->nj());
      vil_convert_planes_to_grey<vxl_byte, float>(*inImg, greyImg);

      //stretch it into 0-1 range
      vil_convert_stretch_range_limited(greyImg, *floatimg, 0.0f, 255.0f, 0.0f, 1.0f);
    }
    pro.set_output_val<vil_image_view_base_sptr>(0, floatimg);
    return true;
  }
  else
    return false;
}
