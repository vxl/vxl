#include "brad_calibration.h"

#include <iomanip>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brad/brad_image_metadata.h>
#include <vil/vil_convert.h>
#include <vnl/vnl_math.h>


vil_image_view<float> brad_nitf_abs_radiometric_calibrate(vil_image_view<unsigned short> const& input_img, brad_image_metadata const& md)
{
  // ***
  // *** INPUT VALIDATION
  // ***

  // number of planes
  unsigned np = input_img.nplanes();
  std::cout << "brad_nitf_abs_radiometric_calibrate" << "-- image plane: " << np << std::endl;

  // check for known band type
  if (md.band_ != "PAN" && md.band_ != "MULTI" && md.band_ != "SWIR") {
    std::ostringstream buffer;
    buffer << "brad_calibration::brad_nitf_abs_radiometric_calibrate: Unknown image band type <" << md.band_ << ">" << std::endl;
    throw std::invalid_argument(buffer.str());
  }

  // calibration factors
  std::vector<double> abscal = md.abscal_;
  std::vector<double> effect_band = md.effect_band_width_;
  if (np != abscal.size() || np != effect_band.size() ) {
    std::ostringstream buffer;
    buffer << "brad_calibration::brad_nitf_abs_radiometric_calibrate: Number of AbsCalFactor/EffectBandWidth values doesn't match number of planes" << std::endl << abscal.size() << " AbsCalFactor, " << effect_band.size() << " EffectBandWidth" << std::endl;
    throw std::invalid_argument(buffer.str());
  }

  std::vector<double> gain = md.gains_;
  std::vector<double> offset = md.offsets_;

  if (np != gain.size() || np != offset.size() ) {
    std::ostringstream buffer;
    buffer << "brad_calibration::brad_nitf_abs_radiometric_calibrate: Number of band dependent gain/offset values doesn't match number of planes" << std::endl << gain.size() << " gains, " << offset.size() << " offsets" << std::endl;
    throw std::invalid_argument(buffer.str());
  }

  // get normalized solar irradiance value from metadata
  std::vector<double> solar_irradiance_val = md.normal_sun_irradiance_values_;
  if (np != solar_irradiance_val.size()) {
    std::ostringstream buffer;
    buffer << "brad_calibration::brad_nitf_abs_radiometric_calibrate: Number of solar irradiance values doesn't match number of planes" << std::endl << solar_irradiance_val.size() << " values" << std::endl;
    throw std::invalid_argument(buffer.str());
  }

  // ***
  // *** CALIBRATION
  // ***

  // Copy input into new float image
  vil_image_view<float> img;
  vil_convert_cast(input_img, img);

  // min/max before calibration
  float min_before, max_before;
  vil_math_value_range(img, min_before, max_before);

  double sun_dot_norm = std::sin(md.sun_elevation_ * vnl_math::pi_over_180);
  for (unsigned ii = 0; ii < np; ii++)
  {
    vil_image_view<float> band = vil_plane(img, ii);

    // absolute calibration
    double abs_cal_factor = abscal[ii] / effect_band[ii];
    vil_math_scale_and_offset_values(band, abs_cal_factor, 0.0);

    // band dependent gain/offset correction
    vil_math_scale_and_offset_values(band, gain[ii], offset[ii]);

    // ToA Radiance to Reflectance
    double band_norm = 1.0 / (solar_irradiance_val[ii] * sun_dot_norm / vnl_math::pi);
    vil_math_scale_values(band, band_norm);

  }

  // min/max after calibration
  float min_after, max_after;
  vil_math_value_range(img, min_after, max_after);

  // report calibration results
  /* std::cout << std::endl */
  /*           << "-----C++ CALIBRATION-----" << std::endl */
  /*           << "BEFORE CALIBRATION: min/max = " */
  /*           << min_before << "/" << max_before << std::endl */
  /*           << "AFTER CALIBRATION: min/max = " */
  /*           << std::setprecision(8) */
  /*           << min_after << "/" << max_after << std::endl; */

  // return new view
  return img;
}

