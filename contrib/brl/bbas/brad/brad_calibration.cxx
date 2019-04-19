#include "brad_nitf_abs_radiometric_calibration.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brad/brad_image_metadata.h>
#include <vil/vil_convert.h>
#include <vnl/vnl_math.h>


vil_image_view_base_sptr brad_nitf_abs_radiometric_calibrate(vil_image_view_base_sptr img_sptr, brad_image_metadata_sptr md) {

  vil_image_view<float> img = *vil_convert_cast(float(), img_sptr);

  float min_val, max_val;
  unsigned np = img.nplanes();
  vil_math_value_range(img, min_val, max_val);
  std::cout << "brad_nitf_abs_radiometric_calibration" << "-- image plane: " << np << ", before calibration img min: " << min_val << " max: " << max_val << std::endl;

  //: perform absolute calibration on image
  std::vector<double> abscal = md->abscal_;
  std::vector<double> effect_band = md->effect_band_width_;
  if (np != abscal.size() || np != effect_band.size() ) {
    std::cerr << "brad_nitf_abs_radiometric_calibration" << "ERROR: Mismatch of image plane number to the length of band dependent AbsCalFactor/EffectBandWidth.  "
                            << "Image plane numebr: " << np
                            << ", gain length: " << abscal.size() << ", offset length: " << effect_band.size() << "!!!\n";
    return nullptr;
  }
  for (unsigned ii = 0; ii < np; ii++)
  {
    vil_image_view<float> band = vil_plane(img, ii);
    double abs_cal_factor = abscal[ii] / effect_band[ii];
    vil_math_scale_and_offset_values(band, abs_cal_factor, 0.0);
  }

  //: perform band dependent gain/offset correction
  if (md->band_ != "PAN" && md->band_ != "MULTI" && md->band_ != "SWIR") {
    std::cout << "brad_nitf_abs_radiometric_calibration" << ": Unknown Image band type " << md->band_ << ", band dependent calibration is ignored." << std::endl;
    vil_math_value_range(img, min_val, max_val);
    std::cout << "brad_nitf_abs_radiometric_calibration" << "after calibration img min: " << min_val << " max: " << max_val << std::endl;
    //output date time info
    return new vil_image_view<float>(img);
  }
  std::vector<double> gain = md->gains_;
  std::vector<double> offset = md->offsets_;

  if (np != gain.size() || np != offset.size() ) {
    std::cerr << "brad_nitf_abs_radiometric_calibration" << "ERROR: Mismatch of image plane number to the length of band dependent gain/offset.  "
                            << "Image plane numebr: " << np
                            << ", gain length: " << gain.size() << ", offset length: " << offset.size() << "!!!\n";
    return nullptr;
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
    std::cerr << "brad_nitf_abs_radiometric_calibration" << "ERROR: Mismatch of image plane numebr to the length of solar irradiance.  "
                            << "Image plane number: " << np
                            << ", solar irradiance value length: " << solar_irradiance_val.size() << "!!!\n";
    return nullptr;
  }
  double sun_dot_norm = std::sin(md->sun_elevation_ * vnl_math::pi_over_180);
  for (unsigned ii = 0; ii< np; ii++)
  {
    double band_norm = 1.0 / (solar_irradiance_val[ii] * sun_dot_norm / vnl_math::pi);
    vil_image_view<float> band = vil_plane(img, ii);
    vil_math_scale_values(band, band_norm);
  }

  vil_math_value_range(img, min_val, max_val);
  std::cout << "brad_nitf_abs_radiometric_calibration" << "after calibration img min: " << min_val << " max: " << max_val << std::endl;

  //output date time info
  return new vil_image_view<float>(img);
}

