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
  vcl_vector<vcl_string> input_types(3);
  input_types[0] = "vil_image_view_base_sptr"; // input image
  input_types[1] = "int"; // ni of target image
  input_types[2] = "int"; // nj of target image
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types(1);
  output_types[0] = "vil_image_view_base_sptr"; // gauss smoothed image(float)
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
bool vil_resample_process(bprb_func_process& pro)
{
  // Sanity check
  if(!pro.verify_inputs())
    return false;

  // get the inputs
  //Retrieve image from input
  vil_image_view_base_sptr input_image = 
    pro.get_input<vil_image_view_base_sptr>(0);

  //Retieve target ni
  int tni = pro.get_input<int>(1);

  //Retieve target nj
  int tnj = pro.get_input<int>(2);

  // convert input image to float in range from 0 to 1
  vil_image_view<float> fimage = *vil_convert_cast(float(), input_image);
  if (input_image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(fimage,1.0/255.0);
  if (input_image->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
    vil_math_scale_values(fimage,1.0/65536);
  unsigned nplanes = fimage.nplanes();
  vil_image_view<float>* out_img = new vil_image_view<float>(tni, tnj, nplanes);
  vil_resample_bilin(fimage, *out_img, tni, tnj);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img);
  return true;
}

