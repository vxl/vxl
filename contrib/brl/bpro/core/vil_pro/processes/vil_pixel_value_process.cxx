// This is brl/bpro/core/vil_pro/processes/vil_pixel_value_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Compute mean value in an image

#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_rgb.h>


//: Constructor
bool vil_pixel_value_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs and returns 1 output
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("int");
  input_types.emplace_back("int");

  std::vector<std::string> output_types;
  output_types.emplace_back("float");  // pixel value
  return pro.set_input_types(input_types)
     &&  pro.set_output_types(output_types);
}

//: Execute the process
bool vil_pixel_value_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()<3) {
    std::cout << "vil_pixel_value_process: The input number should be 3" << std::endl;
    return false;
  }

  // get the input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(0);
  unsigned int i = pro.get_input<int>(1);
  unsigned int j = pro.get_input<int>(2);
  if (i >= img->ni()) {
    pro.set_output_val<float>(0, -1.0f);
    return true;
  }
  if (j >= img->nj()) {
    pro.set_output_val<float>(0, -1.0f);
    return true;
  }

  //byte?
  if (img->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<vxl_byte>& inimg = *dynamic_cast<vil_image_view<vxl_byte>* >(img.ptr());
    if (inimg.nplanes() == 3 || inimg.nplanes() == 4) {
      vil_rgb<vxl_byte> pixel( inimg(i,j,0), inimg(i,j,1), inimg(i,j,2) );
      float pixVal = pixel.grey() / 255.0f;
      pro.set_output_val<float>(0, pixVal);
      return true;
    }
    else {
      float pixVal = inimg(i,j) / 255.0f;
      pro.set_output_val<float>(0, pixVal);
      return true;
    }
  }
  else if (img->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
  {
    vil_image_view<vxl_uint_16>& inimg = *dynamic_cast<vil_image_view<vxl_uint_16>* >(img.ptr());
    float pixVal = inimg(i,j) / 65535.0f;
    pro.set_output_val<float>(0, pixVal);
    return true;
  }
  else if (img->pixel_format() == VIL_PIXEL_FORMAT_INT_16)
  {
    vil_image_view<vxl_int_16>& inimg = *dynamic_cast<vil_image_view<vxl_int_16>* >(img.ptr());
    //float pixVal = inimg(i,j) / 65535.0f;
    float pixVal = inimg(i,j);  // we want the value as is, why scaling??
    pro.set_output_val<float>(0, pixVal);
    return true;
  }
  else if (img->pixel_format() == VIL_PIXEL_FORMAT_FLOAT)
  {
    vil_image_view<float>& inimg = *dynamic_cast<vil_image_view<float>* >(img.ptr());
    pro.set_output_val<float>(0, inimg(i,j));
    return true;
  }
  else if (img->pixel_format() == VIL_PIXEL_FORMAT_BOOL)
  {
    vil_image_view<bool>& inimg = *dynamic_cast<vil_image_view<bool>* >(img.ptr());
    float pixVal = inimg(i,j);
    pro.set_output_val<float>(0, pixVal);
    return true;
  }
  else if (img->pixel_format() == VIL_PIXEL_FORMAT_INT_16)
  {
    vil_image_view<vxl_int_16>& inimg = *dynamic_cast<vil_image_view<vxl_int_16>* >(img.ptr());
    float pixVal = inimg(i,j);
    pro.set_output_val<float>(0, pixVal);
    return true;
  }
  //otherwise it's messed up, return a null pointer
  std::cerr<<"Failed to recognize input image type\n";

  pro.set_output_val<float>(0, -1.0f);
  return true;
}
