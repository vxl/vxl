// This is brl/bpro/core/vil_pro/processes/vil_resample_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/vil_resample_bilin.h>
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_resample_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types(4);
  input_types[0] = "vil_image_view_base_sptr"; // input image
  input_types[1] = "int"; // ni of target image
  input_types[2] = "int"; // nj of target image
  input_types[3] = "vcl_string"; //output type
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types(1);
  output_types[0] = "vil_image_view_base_sptr"; // gauss smoothed image(float)
  ok = pro.set_output_types(output_types);

  //default arguments - returns float
  brdb_value_sptr idx = new brdb_value_t<std::string>("float");
  pro.set_input(3, idx);
  return ok;
}

//: Execute the process
bool vil_resample_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs())
    return false;

  // get the inputs
  //Retrieve image from input
  vil_image_view_base_sptr input_image =
    pro.get_input<vil_image_view_base_sptr>(0);

  //Retieve target ni
  int tni = pro.get_input<int>(1);

  //Retieve target nj
  int tnj = pro.get_input<int>(2);

  //Retrieve output type
  std::string out_type = pro.get_input<std::string>(3);

  ////////////////////////////////////////////////////////////////////
  //Convert to float image
  ////////////////////////////////////////////////////////////////////
  vil_image_view<float> fimage;
  if (input_image->pixel_format() == VIL_PIXEL_FORMAT_RGBA_BYTE ) {
    vil_image_view_base_sptr plane_image = vil_convert_to_n_planes(4, input_image);
    fimage = *vil_convert_cast(float(), plane_image);
    vil_math_scale_values(fimage, 1.0/255.0);
  }
  else {
    //can use convert cast
    fimage = *vil_convert_cast(float(), input_image);

    // convert input image to float in range from 0 to 1
    if (input_image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
      vil_math_scale_values(fimage,1.0/255.0);
    if (input_image->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
      vil_math_scale_values(fimage,1.0/65536);
  }

  ////////////////////////////////////////////////////////////////////
  //resample image
  ////////////////////////////////////////////////////////////////////
  unsigned nplanes = fimage.nplanes();
  auto* out_img = new vil_image_view<float>(tni, tnj, nplanes);
  vil_resample_bilin(fimage, *out_img, tni, tnj);

  ////////////////////////////////////////////////////////////////////
  //return image of same type (convert back)
  ////////////////////////////////////////////////////////////////////
  if (out_type=="float") {
      pro.set_output_val<vil_image_view_base_sptr>(0, out_img);
      return true;
  }
  else if (out_type=="byte") {
      vil_math_scale_values( *out_img, 255.0 );
      auto* bimage = new vil_image_view<vxl_byte>(tni, tnj, nplanes);
      vil_convert_cast(*out_img, *bimage);
      pro.set_output_val<vil_image_view_base_sptr>(0, bimage);
      delete out_img;
      return true;
  }
  else if (out_type=="uint16") {
      vil_math_scale_values( *out_img, 65535.0 );
      auto* bimage = new vil_image_view<vxl_uint_16>(tni, tnj, nplanes);
      vil_convert_cast(*out_img, *bimage);
      pro.set_output_val<vil_image_view_base_sptr>(0, bimage);
      delete out_img;
      return true;
  }
  else if (out_type=="rgba") {
      vil_math_scale_values(*out_img, 255.0);
      auto* bimage = new vil_image_view<vxl_byte>(tni, tnj, nplanes);
      vil_convert_cast(*out_img, *bimage);
      pro.set_output_val<vil_image_view_base_sptr>(0, vil_convert_to_component_order(bimage));
      delete out_img;
      return true;
  }

  //default - return float imgae
  std::cout<<"vil_resample_process::unrecognized output type, returning float image"<<std::endl;
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img);
  return true;
}
