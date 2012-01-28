//This is brl/bpro/core/brad_pro/processes/brad_nitf_abs_radiometric_calibration_process.cxx
//:
// \file
//   Satellite images usually require an absolute radiometric calibration:
//      http://www.digitalglobe.com/downloads/QuickBird_technote_raduse_v1.pdf
//   In the output image, the pixel values are top-of-atmosphere  band-integrated  radiance values 
//     with unit W m^-2 sr^-1 um^-1
//
//

#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <bprb/bprb_func_process.h>

//#include <vul/vul_awk.h>
//#include <vul/vul_file.h>
#include <vil/vil_convert.h>

#include <brad/brad_image_metadata.h>

//: set input and output types
bool brad_nitf_abs_radiometric_calibration_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr"); // cropped satellite image, 
  input_types.push_back("brad_image_metadata_sptr"); 
  if (!pro.set_input_types(input_types))
    return false;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr"); // bits/pixel
  return pro.set_output_types(output_types);
}

bool brad_nitf_abs_radiometric_calibration_process(bprb_func_process& pro)
{

  if (pro.n_inputs()<1)
  {
    vcl_cout << pro.name() << " The input number should be " << 1 << vcl_endl;
    return false;
  }

  //get the inputs
  vil_image_view_base_sptr img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  brad_image_metadata_sptr md = pro.get_input<brad_image_metadata_sptr>(1);

  vil_image_view<float> img = *vil_convert_cast(float(), img_sptr);
  vil_math_scale_and_offset_values(img, md->gain_, md->offset_);

  //output date time info
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(img));
  return true;
}

