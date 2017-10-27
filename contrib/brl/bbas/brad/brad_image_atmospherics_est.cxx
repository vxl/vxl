#include "brad_image_atmospherics_est.h"
//
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <bsta/bsta_histogram.h>
#include <vnl/vnl_math.h>

#include <vil/vil_save.h> // temp
#include <vil/vil_convert.h>
#include <fstream>

bool brad_estimate_atmospheric_parameters(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters &params)
{
  unsigned int ni = radiance.ni();
  unsigned int nj = radiance.nj();
  // find min and max values in image
  float minval, maxval;
  vil_math_value_range(radiance, minval, maxval);
  // compute histogram for image
  bsta_histogram<double> h(minval, maxval, 512);
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
      h.upcount(radiance(i, j), 1.0f);

  // compute airlight
  float frac = 0.0001f;
  double airlight = h.value_with_area_below(frac);

  std::cout << "min = " << minval << ", airlight = " << airlight << std::endl;

  // fix optical depth and skylight
  double optical_depth = 0.10;
  double skylight = 0.0;

  params.airlight_ = airlight;
  params.optical_depth_ = optical_depth;
  params.skylight_ = skylight;

  return true;
}

bool brad_estimate_atmospheric_parameters(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, float mean_reflectance, brad_atmospheric_parameters &params, bool constrain_atmospheric_params)
{
  unsigned int ni = radiance.ni();
  unsigned int nj = radiance.nj();
  // find min and max values in image
  float minval, maxval;
  vil_math_value_range(radiance, minval, maxval);
  // compute histogram for image
  bsta_histogram<double> h(minval, maxval, 512);
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
      h.upcount(radiance(i, j), 1.0f);

  // compute airlight
  float frac = 0.0001f;
  double airlight = h.value_with_area_below(frac);

  std::cout << "min = " << minval << ", airlight = " << airlight << std::endl;

  // find image mean
  double radiance_mean;
  vil_math_mean(radiance_mean, radiance, 0);

  // check the value of image mean and airlight
  if (radiance_mean < airlight) {
    std::cout << "ERROR: radiance mean is less than the airlight; radiance_mean=" << radiance_mean << " airlight=" << airlight << std::endl;
    return false;
  }

  // fix skylight
  double skylight = 0.0;
  double deg2rad = vnl_math::pi_over_180;
  double optical_depth = -std::log(vnl_math::pi / (mean_reflectance * mdata.sun_irradiance_ * std::sin(deg2rad*mdata.sun_elevation_)) * (radiance_mean - airlight));
  optical_depth /= (1.0 / std::sin(deg2rad*mdata.view_elevation_) + 1.0 / std::sin(deg2rad*mdata.sun_elevation_));

  if (constrain_atmospheric_params) {
    // Optical depth cannot be less than 0.
    // in practice, we may not have reliable metadata, in which case the best we can hope for is
    // a reasonable normalization of image intensities - optical depth may need to be < 0 in this case.
    optical_depth = std::max(0.0, optical_depth);
  }

  params.airlight_ = airlight;
  params.optical_depth_ = optical_depth;
  params.skylight_ = skylight;

  std::cout << "airlight = " << airlight << " optical_depth = " << optical_depth << " skylight = " << skylight << std::endl;

  return true;
}

// compute the params from a multi band image
bool brad_estimate_atmospheric_parameters_multi(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters &params)
{
  unsigned int ni = radiance.ni();
  unsigned int nj = radiance.nj();
  unsigned int np = radiance.nplanes();

  std::vector<vil_image_view<float> > bands;
  bands.clear();

  for (unsigned p = 0; p < np; p++)
    bands.push_back(vil_image_view<float>(vil_plane(radiance, p)));

  // calculate atmospheric parameters for each band
  for (unsigned p = 0; p < np; p++)
  {
    // find min and max values in image
    float minval, maxval;
    vil_math_value_range(bands[p], minval, maxval);
    // compute histogram for image
    bsta_histogram<double> h(minval, maxval, 512);
    for (unsigned j = 0; j < nj; j++)
      for (unsigned i = 0; i < ni; i++)
        h.upcount(bands[p](i, j), 1.0f);

    // compute airlight
    float frac = 0.0001f;
    double airlight = h.value_with_area_below(frac);
    // fix optical depth and skylight
    double optical_depth = 0.0;
    double skylight = 0.0;
    std::cout << "band " << p << ": min = " << minval << ", airlight = " << airlight
      << ", optical_depth (fixed) = " << optical_depth << ", skylight (fixed) = " << skylight << std::endl;
    // set atmospheric parameters
    params.airlight_multi_.push_back(airlight);
    params.optical_depth_multi_.push_back(optical_depth);
    params.skylight_multi_.push_back(skylight);
  }
  return true;
}

// compute the params from a multi band image - only for 4 bands for now
bool brad_estimate_atmospheric_parameters_multi(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, float mean_reflectance, brad_atmospheric_parameters &params, bool constrain_atmospheric_params)
{
  unsigned ni = radiance.ni();
  unsigned nj = radiance.nj();
  unsigned np = radiance.nplanes();

  // calculate atmospheric parameters for each band
  for (unsigned p = 0; p < np; p++)
  {
    vil_image_view<float> img = vil_plane(radiance, p);
    float minval, maxval;
    vil_math_value_range(img, minval, maxval);
    // compute histogram for image band p
    bsta_histogram<double> h(minval, maxval, 512);
    for (unsigned j = 0; j < nj; j++)
      for (unsigned i = 0; i < ni; i++)
        h.upcount(img(i, j), 1.0f);
    // compute airlight
    float frac = 0.0001f;
    double airlight = h.value_with_area_below(frac);

    // find image mean
    double radiance_mean;
    vil_math_mean(radiance_mean, img, 0);
    // check the image mean value and the computed airlight
    if (radiance_mean < airlight) {
      std::cerr << "brad_image_atmospherics_est::brad_estimate_atmospheric_parameters_multi : radiance_mean " << radiance_mean << " is smaller than airlight " << airlight << "\n!";
      return false;
    }
    // fix skylight
    double skylight = 0.0;
    // calculate optical depth
    double deg2rad = vnl_math::pi_over_180;
    double optical_depth = -std::log(vnl_math::pi / (mean_reflectance*mdata.sun_irradiance_values_[p] * std::sin(deg2rad*mdata.sun_elevation_)) * (radiance_mean - airlight));
    optical_depth /= (1.0 / std::sin(deg2rad*mdata.view_elevation_) + 1.0 / std::sin(deg2rad*mdata.sun_elevation_));


    if (constrain_atmospheric_params) {
      // Optical depth cannot be less than 0
      // in practice, we may not have reliable metadata, in which case the best we can hope for is
      // a reasonable normalization of image intensities - optical depth may need to be < 0 in this case.
      optical_depth = std::max(0.0, optical_depth);
    }
    // set parameters
    params.airlight_multi_.push_back(airlight);
    params.optical_depth_multi_.push_back(optical_depth);
    params.skylight_multi_.push_back(skylight);
    std::cout << "band " << p << ": min = " << minval << ", airlight = " << airlight
      << ", optical_depth = " << optical_depth << ", skylight (fixed) = " << skylight << std::endl;
  }
  return true;
}

// compute the params from a multi band image
bool brad_estimate_atmospheric_parameters_multi_average(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, float mean_reflectance, brad_atmospheric_parameters &params, int dark_pixels_band, bool constrain_atmospheric_params)
{
  unsigned ni = radiance.ni();
  unsigned nj = radiance.nj();
  unsigned np = radiance.nplanes();

  float frac = 0.0001f;
  // create average image
  vil_image_view<float> ave_im;
  ave_im.set_size(ni, nj);
  ave_im.fill(0.0);
  for (int j = 0; j < nj; j++) {
    for (int i = 0; i < ni; i++) {
      for (int p = 0; p < dark_pixels_band; p++) {//for (int p = 0; p < np; p++) {
        ave_im(i, j) += radiance(i, j, p);
      }
    }
  }

  float minval, maxval;
  vil_math_value_range(ave_im, minval, maxval);

  // compute histogram for image band p
  bsta_histogram<double> h(minval, maxval, 512);
    for (unsigned j = 0; j < nj; j++)
      for (unsigned i = 0; i < ni; i++)
        h.upcount(ave_im(i, j), 1.0f);
  double airlight_ave = h.value_with_area_below(frac);

  // calculate atmospheric parameters for each band
  for (unsigned p = 0; p < np; p++)
  {
 
    vil_image_view<float> img = vil_plane(radiance, p);

    double airlight = 0;
    int count = 0;
    //compute airlight for this plane
    for (int j = 0; j < nj; j++) {
      for (int i = 0; i < ni; i++) {
        if (ave_im(i, j) < airlight_ave) {
          airlight += img(i, j);
          count++;
        }
      }
    }
    airlight /= (double) count;
    // find image mean
    double radiance_mean;
    vil_math_mean(radiance_mean, img, 0);
    // check the image mean value and the computed airlight 
    if (radiance_mean < airlight) {
      std::cerr << "brad_image_atmospherics_est::brad_estimate_atmospheric_parameters_multi : radiance_mean " << radiance_mean << " is smaller than airlight " << airlight << "\n!";
      return false;
    }
    // fix skylight
    double skylight = 0.0;
    // calculate optical depth
    double deg2rad = vnl_math::pi_over_180;
    double optical_depth = -std::log(vnl_math::pi / (mean_reflectance*mdata.sun_irradiance_values_[p] * std::sin(deg2rad*mdata.sun_elevation_)) * (radiance_mean - airlight));
    optical_depth /= (1.0 / std::sin(deg2rad*mdata.view_elevation_) + 1.0 / std::sin(deg2rad*mdata.sun_elevation_));


    if (constrain_atmospheric_params) {
      // Optical depth cannot be less than 0
      // in practice, we may not have reliable metadata, in which case the best we can hope for is
      // a reasonable normalization of image intensities - optical depth may need to be < 0 in this case.
      optical_depth = std::max(0.0, optical_depth);
    }
    // set parameters
    params.airlight_multi_.push_back(airlight);
    params.optical_depth_multi_.push_back(optical_depth);
    params.skylight_multi_.push_back(skylight);
    std::cout << "band " << p << ": min = " << minval << ", airlight = " << airlight
      << ", optical_depth = " << optical_depth << ", skylight (fixed) = " << skylight << std::endl;
  }
  return true;
}

bool brad_undo_reflectance_estimate(vil_image_view<float> const& reflectance, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &radiance)
{
  // calculate atmosphere transmittance value
  double sun_el_rads = mdata.sun_elevation_ * vnl_math::pi_over_180;
  double sat_el_rads = mdata.view_elevation_ * vnl_math::pi_over_180;

  double T_sun = std::exp(-atm_params.optical_depth_ / std::sin(sun_el_rads));
  double T_view = std::exp(-atm_params.optical_depth_ / std::sin(sat_el_rads));

  // ideal Lambertian reflector, surface normal = [0 0 1]
  double sun_dot_norm = std::sin(sun_el_rads);
  double Lsat_horizontal = T_view*(mdata.sun_irradiance_ * sun_dot_norm * T_sun + atm_params.skylight_) / vnl_math::pi + atm_params.airlight_;

  // convert reflectance values to radiance
  unsigned int ni = reflectance.ni();
  unsigned int nj = reflectance.nj();
  radiance.set_size(ni, nj);
  for (unsigned int j = 0; j<nj; ++j) {
    for (unsigned int i = 0; i<ni; ++i) {
      radiance(i, j) = float(reflectance(i, j) * Lsat_horizontal + atm_params.airlight_);
    }
  }
  return true;
}


bool brad_estimate_reflectance_image(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &reflectance)
{
  // calculate atmosphere transmittance value
  double sun_el_rads = mdata.sun_elevation_ * vnl_math::pi_over_180;
  double sat_el_rads = mdata.view_elevation_ * vnl_math::pi_over_180;

  double T_sun = std::exp(-atm_params.optical_depth_ / std::sin(sun_el_rads));
  double T_view = std::exp(-atm_params.optical_depth_ / std::sin(sat_el_rads));

  // ideal Lambertian reflector, surface normal = [0 0 1]
  double sun_dot_norm = std::sin(sun_el_rads);
  double Lsat_horizontal = T_view*(mdata.sun_irradiance_ * sun_dot_norm * T_sun + atm_params.skylight_) / vnl_math::pi + atm_params.airlight_;

  // stretch radiance values such that airlight value maps to 0, and ideal reflector maps to 1
  unsigned int ni = radiance.ni();
  unsigned int nj = radiance.nj();
  reflectance.set_size(ni, nj);
  for (unsigned int j = 0; j<nj; ++j) {
    for (unsigned int i = 0; i<ni; ++i) {
      double normalized = (radiance(i, j) - atm_params.airlight_) / Lsat_horizontal;
      // don't let reflectance value fall below 0
      // value above 1 is unlikely but possible (e.g. specular reflection)
      reflectance(i, j) = std::max(0.0f, float(normalized));
    }
  }
  return true;
}

bool brad_estimate_reflectance_image_multi(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &reflectance_multi)
{
  // calculate atmosphere transmittance value
  double sun_el_rads = mdata.sun_elevation_  * vnl_math::pi_over_180;
  double sat_el_rads = mdata.view_elevation_ * vnl_math::pi_over_180;
  unsigned ni = radiance.ni();
  unsigned nj = radiance.nj();
  unsigned np = radiance.nplanes();

  for (unsigned p = 0; p < np; p++)
  {
    vil_image_view<float> img = vil_plane(radiance, p);
    vil_image_view<float> reflectance = vil_plane(reflectance_multi, p);

    double T_sun = std::exp(-atm_params.optical_depth_multi_[p] / std::sin(sun_el_rads));
    double T_view = std::exp(-atm_params.optical_depth_multi_[p] / std::sin(sat_el_rads));

    // ideal Lambertian reflector, surface normal = [0,0,1]
    double sun_dot_norm = std::sin(sun_el_rads);
    double Lsat_horizontal = T_view*(mdata.sun_irradiance_values_[p] * sun_dot_norm*T_sun + atm_params.skylight_multi_[p]) / vnl_math::pi;// +atm_params.airlight_multi_[p];

    // stretch radiance values such that airlight value maps to 0, and ideal reflector maps to 1
    for (unsigned j = 0; j < nj; j++) {
      for (unsigned i = 0; i < ni; i++) {
        double normalized = (img(i, j) - atm_params.airlight_multi_[p]) / Lsat_horizontal;
        // don't let reflectance value fall below 0
        // value above 1 is unlikely but possible (e.g. specular reflection)
        reflectance(i, j) = std::min(std::max(0.0f, float(normalized)), 1.0f);
      }
    }
  }
  return true;
}

//---------------------------------------------------------------------------
//: Estimate the reflectance image without needing to access the meta data
// should work for panchromatic or multispectral images
//---------------------------------------------------------------------------
bool brad_estimate_reflectance_image_multi(
  vil_image_view<float> const& radiance, 
  float mean_reflectance, 
  vil_image_view<float> &cal_img, 
  int min_norm_band, 
  int max_norm_band)
{
  // set default min/max normalization and dark pixel bands
  if (min_norm_band < 0 || max_norm_band < 0) { // wv3
    if (radiance.nplanes() == 8 || radiance.nplanes() == 16) {
      min_norm_band = 1;
      max_norm_band = 4;
    }
    else if (radiance.nplanes() == 1) { // panchromatic
      min_norm_band = 0;
      max_norm_band = 0;
    }
    else {
      std::cout << "WARNING: unrecognized number of spectral bands; using first band for normalization and dark pixel calculation\n";
      min_norm_band = 0;
      max_norm_band = 0;
    }
  }
  unsigned ni = radiance.ni();
  unsigned nj = radiance.nj();
  unsigned np = radiance.nplanes();

  float frac = 0.001f; //0.0001
  // create average image
  vil_image_view<float> ave_im;
  ave_im.set_size(ni, nj);
  ave_im.fill(0.0);
  for (int j = 0; j < nj; j++) {
    for (int i = 0; i < ni; i++) {
      for (int p = min_norm_band; p < max_norm_band + 1; p++) {//for (int p = 0; p < np; p++) {
        ave_im(i, j) += radiance(i, j, p);
      }
    }
  }

  float minval, maxval;
  vil_math_value_range(ave_im, minval, maxval);

  // compute histogram for image band p
  bsta_histogram<double> h(minval, maxval, 512);
  for (unsigned j = 0; j < nj; j++)
    for (unsigned i = 0; i < ni; i++)
      h.upcount(ave_im(i, j), 1.0f);
  double airlight_ave = h.value_with_area_below(frac);

  // calculate atmospheric parameters for each band
  std::vector<float> airlight;
  airlight.resize(np);
  for (unsigned p = 0; p < np; p++)
  {
    vil_image_view<float> img = vil_plane(radiance, p);
    double airlight_sum = 0.0;
    int count = 0;
    //compute airlight for this plane
    for (int j = 0; j < nj; j++) {
      for (int i = 0; i < ni; i++) {
        if (ave_im(i, j) < airlight_ave) {
          airlight_sum += (float)img(i, j);
          count++;
        }
      }
    }
    airlight[p] = (float)(airlight_sum / count);
  }

  // compute Lsat_horizontal average using the planes specified in planes_for_average
  double Lsat_horizontal = 0;
  for (int p = min_norm_band; p < max_norm_band + 1; p++) {
    vil_image_view<float> cur_plane = vil_plane(radiance, p);
    double plane_ave = 0;
    for (int j = 0; j < cur_plane.nj(); j++) {
      for (int i = 0; i < cur_plane.ni(); i++) {
        plane_ave += double(cur_plane(i, j));
      }
    }
    plane_ave /= cur_plane.ni()*cur_plane.nj();
    Lsat_horizontal += (plane_ave - airlight[p]);
  }
  Lsat_horizontal /= (max_norm_band - min_norm_band + 1);

  float norm_factor = (float)(mean_reflectance / Lsat_horizontal);

  cal_img.set_size(ni, nj, np);
  cal_img.fill(0.0);
  // apply image correction
  for (int p = 0; p < radiance.nplanes(); p++) {
    for (int j = 0; j < radiance.nj(); j++) {
      for (int i = 0; i < radiance.ni(); i++) {
        double normalized = (double(radiance(i, j, p)) - airlight[p]) * norm_factor;
        cal_img(i, j, p) = std::max(0.0f, float(normalized));
      }
    }
  }
  return true;
}


//------------------------------------------------------------------
bool brad_atmo_radiance_to_reflectance(
  vil_image_view<float> const& radiance,
  brad_image_metadata const& mdata,
  vil_image_view<float> &reflectance )
{
  if (radiance.nplanes() != mdata.sun_irradiance_values_.size()) return false;

  double sun_dot_norm = std::sin(mdata.sun_elevation_*vnl_math::pi_over_180);

  for (unsigned b = 0; b < radiance.nplanes(); b++){

    double band_norm = 1.0/(
      mdata.sun_irradiance_values_[b] * sun_dot_norm / vnl_math::pi);

    vil_image_view<float> rad_band = vil_plane(radiance, b);
    vil_image_view<float> ref_band = vil_plane(reflectance, b);
    ref_band.deep_copy(rad_band);
    vil_math_scale_values(ref_band, band_norm);
  }
  return true;

}