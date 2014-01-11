//This is brl/bpro/core/brad_pro/processes/brad_nitf_abs_radiometric_calibration_process.cxx
//:
// \file
//   Satellite images usually require an absolute radiometric calibration:
//      http://www.digitalglobe.com/downloads/QuickBird_technote_raduse_v1.pdf
//   In the output image, the pixel values are top-of-atmosphere  band-averaged radiance values
//     with unit W m^-2 sr^-1 um^-1

#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <bprb/bprb_func_process.h>

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

  float min_val, max_val;
  vil_math_value_range(img, min_val, max_val);
  vcl_cout << "before calibration img min: " << min_val << " max: " << max_val << vcl_endl;

  if (img.nplanes() == 1) {
    //: calibrate
    vil_math_scale_and_offset_values(img, md->gain_, md->offset_);
  } else if (img.nplanes() >= 4) {  // a multi-spectral image
    vil_image_view<float> band1 = vil_plane(img, 0);
    vil_math_scale_and_offset_values(band1, md->gains_[1].first, md->gains_[1].second);  // assuming gains_[0] is PAN band's gain
    vil_image_view<float> band2 = vil_plane(img, 1);
    vil_math_scale_and_offset_values(band2, md->gains_[2].first, md->gains_[2].second);
    vil_image_view<float> band3 = vil_plane(img, 2);
    vil_math_scale_and_offset_values(band3, md->gains_[3].first, md->gains_[3].second);
    vil_image_view<float> band4 = vil_plane(img, 3);
    vil_math_scale_and_offset_values(band4, md->gains_[4].first, md->gains_[4].second);
  } else 
    return false;

  vil_math_value_range(img, min_val, max_val);
  vcl_cout << "after calibration img min: " << min_val << " max: " << max_val << vcl_endl;

  //output date time info
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(img));
  return true;
}

