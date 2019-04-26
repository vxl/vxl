//This is brl/bpro/core/brad_pro/processes/brad_nitf_abs_radiometric_calibration_process.cxx
//:
// \file
//   Satellite images usually require an absolute radiometric calibration:
//      http://www.digitalglobe.com/downloads/QuickBird_technote_raduse_v1.pdf
//   Update reference in 2017
//      https://dg-cms-uploads-production.s3.amazonaws.com/uploads/document/file/209/ABSRADCAL_FLEET_2016v0_Rel20170606.pdf
//   The pixel value in output image is Top of Atmosphere (ToA) reflectance
// \verbatim
//   Yi Dong --- Oct, 2014  added radiometric normalization for different types of multi-spectral images
//   Yi Dong --- Dec, 2017  added band dependent calibration using most updated gain and offset
//   Yi Dong --- Dec, 2017  This process performs 1. image Digital Number to ToA Radiance by calibration; 2. Convert Radiance to ToA Reflectance
// \endverbatim

#include <string>
#include <iostream>
#include <vector>
#include <bprb/bprb_func_process.h>

#include <vil/vil_convert.h>

#include <brad/brad_image_metadata.h>
#include <brad/brad_calibration.h>

//: set input and output types
bool brad_nitf_abs_radiometric_calibration_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); // cropped satellite image,
  input_types.emplace_back("brad_image_metadata_sptr");
  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // bits/pixel
  return pro.set_output_types(output_types);
}

bool brad_nitf_abs_radiometric_calibration_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " Wrong Inputs!!!" << std::endl;
    return false;
  }

  // get the inputs
  vil_image_view_base_sptr img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  brad_image_metadata_sptr md = pro.get_input<brad_image_metadata_sptr>(1);

  // call the function
  vil_image_view<float> output_img = brad_nitf_abs_radiometric_calibrate(*img_sptr, *md);

  // create a smart pointer to return
  vil_image_view_base_sptr output = new vil_image_view<float>(output_img);

  if (output) {
    pro.set_output_val<vil_image_view_base_sptr>(0, output);
    return true;
  } else {
    return false;
  }
}

