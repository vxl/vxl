//This is brl/bpro/core/brad_pro/processes/brad_nitf_abs_radiometric_calibration_process.cxx
//:
// \file
//   Satellite images usually require an absolute radiometric calibration:
//      http://www.digitalglobe.com/downloads/QuickBird_technote_raduse_v1.pdf
//   In the output image, the pixel values are top-of-atmosphere  band-averaged radiance values
//     with unit W m^-2 sr^-1 um^-1
// \verbatim
//   Yi Dong --- Oct, 2014  added radiometric normalization for different types of multi-spectral images
// \endverbatim

#include <vcl_compiler.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#include <vil/vil_convert.h>

#include <brad/brad_image_metadata.h>

//: set input and output types
bool brad_nitf_abs_radiometric_calibration_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.push_back("vil_image_view_base_sptr"); // cropped satellite image,
  input_types.push_back("brad_image_metadata_sptr");
  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  output_types.push_back("vil_image_view_base_sptr"); // bits/pixel
  return pro.set_output_types(output_types);
}

bool brad_nitf_abs_radiometric_calibration_process(bprb_func_process& pro)
{
  if (pro.n_inputs()<1)
  {
    std::cout << pro.name() << " The input number should be " << 1 << std::endl;
    return false;
  }

  //get the inputs
  vil_image_view_base_sptr img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  brad_image_metadata_sptr md = pro.get_input<brad_image_metadata_sptr>(1);

  vil_image_view<float> img = *vil_convert_cast(float(), img_sptr);

  float min_val, max_val;
  vil_math_value_range(img, min_val, max_val);
  std::cout << pro.name() << ": before calibration img min: " << min_val << " max: " << max_val << std::endl;

  //: calibrate
  if (img.nplanes() == 1) {
    vil_math_scale_and_offset_values(img, md->gain_, md->offset_);
  }
  else if (img.nplanes() >= 4)
  {  // a multi-spectral image 4 or 8 bands
    for (unsigned ii = 0; ii < img.nplanes(); ii++) {
      vil_image_view<float> band = vil_plane(img, ii);
      vil_math_scale_and_offset_values(band, md->gains_[ii+1].first, md->gains_[ii+1].second);  // assuming gains_[0] is PAN band's gain (brad_image_metadata parses from metadata files and creates gains_ vector accordingly)
    }
  }
  else
    return false;

  vil_math_value_range(img, min_val, max_val);
  std::cout << pro.name() << "after calibration img min: " << min_val << " max: " << max_val << std::endl;

  //output date time info
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(img));
  return true;
}

