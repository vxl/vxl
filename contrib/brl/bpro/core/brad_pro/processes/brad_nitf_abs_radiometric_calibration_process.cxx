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

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bprb/bprb_func_process.h>
#include <vnl/vnl_math.h>

#include <vil/vil_convert.h>

#include <brad/brad_image_metadata.h>

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

  //get the inputs
  vil_image_view_base_sptr img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  brad_image_metadata_sptr md = pro.get_input<brad_image_metadata_sptr>(1);

  vil_image_view<float> img = *vil_convert_cast(float(), img_sptr);

  float min_val, max_val;
  unsigned np = img.nplanes();
  vil_math_value_range(img, min_val, max_val);
  std::cout << pro.name() << "-- image plane: " << np << ", before calibration img min: " << min_val << " max: " << max_val << std::endl;

  //: perform absolute calibration on image
  std::vector<double> abscal = md->abscal_;
  std::vector<double> effect_band = md->effect_band_width_;
  if (np != abscal.size() || np != effect_band.size() ) {
    std::cerr << pro.name() << "ERROR: Mismatch of image plane number to the length of band dependent AbsCalFactor/EffectBandWidth.  "
                            << "Image plane numebr: " << np
                            << ", gain length: " << abscal.size() << ", offset length: " << effect_band.size() << "!!!\n";
    return false;
  }
  for (unsigned ii = 0; ii < np; ii++)
  {
    vil_image_view<float> band = vil_plane(img, ii);
    double abs_cal_factor = abscal[ii] / effect_band[ii];
    vil_math_scale_and_offset_values(band, abs_cal_factor, 0.0);
  }

  //: perform band dependent gain/offset correction
  if (md->band_ != "PAN" && md->band_ != "MULTI" && md->band_ != "SWIR") {
    std::cout << pro.name() << ": Unknown Image band type " << md->band_ << ", band dependent calibration is ignored." << std::endl;
    vil_math_value_range(img, min_val, max_val);
    std::cout << pro.name() << "after calibration img min: " << min_val << " max: " << max_val << std::endl;
    //output date time info
    pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(img));
    return true;
  }
  std::vector<double> gain = md->gains_;
  std::vector<double> offset = md->offsets_;

  if (np != gain.size() || np != offset.size() ) {
    std::cerr << pro.name() << "ERROR: Mismatch of image plane number to the length of band dependent gain/offset.  "
                            << "Image plane numebr: " << np
                            << ", gain length: " << gain.size() << ", offset length: " << offset.size() << "!!!\n";
    return false;
  }
  for (unsigned ii = 0; ii < np; ii++)
  {
    vil_image_view<float> band = vil_plane(img, ii);
    vil_math_scale_and_offset_values(band, gain[ii], offset[ii]);
  }


  // perform ToA Radiance to Reflectance
  // get normalized solar irradiance value from metadata
  std::vector<double> solar_irradiance_val = md->normal_sun_irradiance_values_;
  if (np != solar_irradiance_val.size()) {
    std::cerr << pro.name() << "ERROR: Mismatch of image plane numebr to the length of solar irradiance.  "
                            << "Image plane number: " << np
                            << ", solar irradiance value length: " << solar_irradiance_val.size() << "!!!\n";
    return false;
  }
  double sun_dot_norm = std::sin(md->sun_elevation_ * vnl_math::pi_over_180);
  for (unsigned ii = 0; ii< np; ii++)
  {
    double band_norm = 1.0 / (solar_irradiance_val[ii] * sun_dot_norm / vnl_math::pi);
    vil_image_view<float> band = vil_plane(img, ii);
    vil_math_scale_values(band, band_norm);
  }

  vil_math_value_range(img, min_val, max_val);
  std::cout << pro.name() << "after calibration img min: " << min_val << " max: " << max_val << std::endl;

  //output date time info
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(img));
  return true;
}
