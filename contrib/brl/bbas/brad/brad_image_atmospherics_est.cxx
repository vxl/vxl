#include "brad_image_atmospherics_est.h"
//
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <bsta/bsta_histogram.h>
#include <vnl/vnl_math.h>

bool brad_estimate_atmospheric_parameters(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters &params)
{
   unsigned int ni = radiance.ni();
   unsigned int nj = radiance.nj();
  // find min and max values in image
  float minval, maxval;
  vil_math_value_range(radiance,minval,maxval);
  // compute histogram for image
  bsta_histogram<float> h(minval, maxval, 512);
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
      h.upcount(radiance(i, j), 1.0f);

  // compute airlight
  float frac = 0.0001f;
  double airlight = h.value_with_area_below(frac);

  vcl_cout << "min = " << minval << ", airlight = " << airlight << vcl_endl;

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
  vil_math_value_range(radiance,minval,maxval);
  // compute histogram for image
  bsta_histogram<float> h(minval, maxval, 512);
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
      h.upcount(radiance(i, j), 1.0f);

  // compute airlight
  float frac = 0.0001f;
  double airlight = h.value_with_area_below(frac);

  vcl_cout << "min = " << minval << ", airlight = " << airlight << vcl_endl;

  // find image mean
  float radiance_mean;
  vil_math_mean(radiance_mean, radiance, 0);

  // check the value of image mean and airlight
  if (radiance_mean < airlight)
    return false;

  // fix skylight
  double skylight = 0.0;
  // check the value is valid
  if (radiance_mean <= airlight) {
    return false;
  }

  double deg2rad = vnl_math::pi_over_180;
  double optical_depth = -vcl_log(vnl_math::pi / (mean_reflectance * mdata.sun_irradiance_ * vcl_sin(deg2rad*mdata.sun_elevation_)) * (radiance_mean - airlight));
  optical_depth /= (1.0/vcl_sin(deg2rad*mdata.view_elevation_) + 1.0/vcl_sin(deg2rad*mdata.sun_elevation_));

  if (constrain_atmospheric_params) {
    // Optical depth cannot be less than 0.
    // in practice, we may not have reliable metadata, in which case the best we can hope for is
    // a reasonable normalization of image intensities - optical depth may need to be < 0 in this case.
    optical_depth = vcl_max(0.0, optical_depth);
  }

  params.airlight_ = airlight;
  params.optical_depth_ = optical_depth;
  params.skylight_ = skylight;

  vcl_cout << "airlight = " << airlight << " optical_depth = " << optical_depth << " skylight = " << skylight << vcl_endl;

  return true;
}

// compute the params from a multi band image - only 4 bands for now
bool brad_estimate_atmospheric_parameters_multi(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters &params)
{
  unsigned int ni = radiance.ni();
  unsigned int nj = radiance.nj();
  
  vil_image_view<float> band1 = vil_plane(radiance, 0);
  vil_image_view<float> band2 = vil_plane(radiance, 1);
  vil_image_view<float> band3 = vil_plane(radiance, 2);
  vil_image_view<float> band4 = vil_plane(radiance, 3);
  
  vcl_vector<vil_image_view<float> > imgs;
  imgs.push_back(band1); 
  imgs.push_back(band2);
  imgs.push_back(band3);
  imgs.push_back(band4);

  for (unsigned ii = 0; ii < 4; ii++) {
    // find min and max values in image
    float minval, maxval;
    vil_math_value_range(imgs[ii],minval,maxval);
    // compute histogram for image
    bsta_histogram<float> h(minval, maxval, 512);
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
        h.upcount(imgs[ii](i, j), 1.0f);

    // compute airlight
    float frac = 0.0001f;
    double airlight = h.value_with_area_below(frac);

    vcl_cout << "min = " << minval << ", airlight = " << airlight << vcl_endl;

    // fix optical depth and skylight
    double optical_depth = 0.10;
    double skylight = 0.0;

    params.airlight_multi_.push_back(airlight);
    params.optical_depth_multi_.push_back(optical_depth);
    params.skylight_multi_.push_back(skylight);
  }

  return true;
}

// compute the params from a multi band image - only for 4 bands for now
bool brad_estimate_atmospheric_parameters_multi(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, float mean_reflectance, brad_atmospheric_parameters &params, bool constrain_atmospheric_params)
{
  if (radiance.nplanes() < 4)
    return false;

  unsigned int ni = radiance.ni();
  unsigned int nj = radiance.nj();
  
  vil_image_view<float> band1 = vil_plane(radiance, 0);
  vil_image_view<float> band2 = vil_plane(radiance, 1);
  vil_image_view<float> band3 = vil_plane(radiance, 2);
  vil_image_view<float> band4 = vil_plane(radiance, 3);
  vcl_vector<vil_image_view<float> > imgs;
  imgs.push_back(band1); 
  imgs.push_back(band2);
  imgs.push_back(band3);
  imgs.push_back(band4);
  
  for (unsigned ii = 0; ii < 4; ii++) {
    float minval, maxval;
    vil_math_value_range(imgs[ii],minval,maxval);
    // compute histogram for image
    bsta_histogram<float> h(minval, maxval, 512);
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
        h.upcount(imgs[ii](i, j), 1.0f);

    // compute airlight
    float frac = 0.0001f;
    double airlight = h.value_with_area_below(frac);
    vcl_cout << "min = " << minval << ", airlight = " << airlight << vcl_endl;

    // find image mean
    float radiance_mean;
    vil_math_mean(radiance_mean, imgs[ii], 0);

    // check the image mean value and the computed airlight
    if (radiance_mean < airlight)
      return false;

    // fix skylight
    double skylight = 0.0;


    double deg2rad = vnl_math::pi_over_180;
    double optical_depth = -vcl_log(vnl_math::pi / (mean_reflectance * mdata.sun_irradiance_values_[ii] * vcl_sin(deg2rad*mdata.sun_elevation_)) * (radiance_mean - airlight));
    optical_depth /= (1.0/vcl_sin(deg2rad*mdata.view_elevation_) + 1.0/vcl_sin(deg2rad*mdata.sun_elevation_));

    if (constrain_atmospheric_params) {
      // Optical depth cannot be less than 0.
      // in practice, we may not have reliable metadata, in which case the best we can hope for is
      // a reasonable normalization of image intensities - optical depth may need to be < 0 in this case.
      optical_depth = vcl_max(0.0, optical_depth);
    }

    params.airlight_multi_.push_back(airlight);
    params.optical_depth_multi_.push_back(optical_depth);
    params.skylight_multi_.push_back(skylight);

    vcl_cout << "airlight = " << airlight << " optical_depth = " << optical_depth << " skylight = " << skylight << vcl_endl;
  }

  return true;
}


bool brad_undo_reflectance_estimate(vil_image_view<float> const& reflectance, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &radiance)
{
  // calculate atmosphere transmittance value
  double sun_el_rads = mdata.sun_elevation_ * vnl_math::pi_over_180;
  double sat_el_rads = mdata.view_elevation_ * vnl_math::pi_over_180;

  double T_sun = vcl_exp(-atm_params.optical_depth_ / vcl_sin(sun_el_rads));
  double T_view = vcl_exp(-atm_params.optical_depth_ / vcl_sin(sat_el_rads));

  // ideal Lambertian reflector, surface normal = [0 0 1]
  double sun_dot_norm = vcl_sin(sun_el_rads);
  double Lsat_horizontal = T_view*(mdata.sun_irradiance_ * sun_dot_norm * T_sun + atm_params.skylight_)/vnl_math::pi + atm_params.airlight_;

  // convert reflectance values to radiance
  unsigned int ni = reflectance.ni();
  unsigned int nj = reflectance.nj();
  radiance.set_size(ni,nj);
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      radiance(i,j) = float(reflectance(i,j) * Lsat_horizontal + atm_params.airlight_);
    }
  }
  return true;
}


bool brad_estimate_reflectance_image(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &reflectance)
{
  // calculate atmosphere transmittance value
  double sun_el_rads = mdata.sun_elevation_ * vnl_math::pi_over_180;
  double sat_el_rads = mdata.view_elevation_ * vnl_math::pi_over_180;

  double T_sun = vcl_exp(-atm_params.optical_depth_ / vcl_sin(sun_el_rads));
  double T_view = vcl_exp(-atm_params.optical_depth_ / vcl_sin(sat_el_rads));

  // ideal Lambertian reflector, surface normal = [0 0 1]
  double sun_dot_norm = vcl_sin(sun_el_rads);
  double Lsat_horizontal = T_view*(mdata.sun_irradiance_ * sun_dot_norm * T_sun + atm_params.skylight_)/vnl_math::pi + atm_params.airlight_;

  // stretch radiance values such that airlight value maps to 0, and ideal reflector maps to 1
  unsigned int ni = radiance.ni();
  unsigned int nj = radiance.nj();
  reflectance.set_size(ni,nj);
  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      double normalized = (radiance(i,j) - atm_params.airlight_) / Lsat_horizontal;
      // don't let reflectance value fall below 0
      // value above 1 is unlikely but possible (e.g. specular reflection)
      reflectance(i,j) = vcl_max(0.0f, float(normalized));
    }
  }
  return true;
}

bool brad_estimate_reflectance_image_multi(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &reflectance_multi)
{

  vil_image_view<float> band1 = vil_plane(radiance, 0);
  vil_image_view<float> band2 = vil_plane(radiance, 1);
  vil_image_view<float> band3 = vil_plane(radiance, 2);
  vil_image_view<float> band4 = vil_plane(radiance, 3);
  vcl_vector<vil_image_view<float> > imgs;
  imgs.push_back(band1); 
  imgs.push_back(band2);
  imgs.push_back(band3);
  imgs.push_back(band4);

  // calculate atmosphere transmittance value
  double sun_el_rads = mdata.sun_elevation_ * vnl_math::pi_over_180;
  double sat_el_rads = mdata.view_elevation_ * vnl_math::pi_over_180;

  for (unsigned ii = 0; ii < 4; ii++) {
    vil_image_view<float> reflectance = vil_plane(reflectance_multi, ii);
    
    double T_sun = vcl_exp(-atm_params.optical_depth_multi_[ii] / vcl_sin(sun_el_rads));
    double T_view = vcl_exp(-atm_params.optical_depth_multi_[ii] / vcl_sin(sat_el_rads));

    // ideal Lambertian reflector, surface normal = [0 0 1]
    double sun_dot_norm = vcl_sin(sun_el_rads);
    double Lsat_horizontal = T_view*(mdata.sun_irradiance_values_[ii] * sun_dot_norm * T_sun + atm_params.skylight_multi_[ii])/vnl_math::pi + atm_params.airlight_multi_[ii];

    // stretch radiance values such that airlight value maps to 0, and ideal reflector maps to 1
    unsigned int ni = imgs[ii].ni();
    unsigned int nj = imgs[ii].nj();
    for (unsigned int j=0; j<nj; ++j) {
      for (unsigned int i=0; i<ni; ++i) {
        double normalized = (imgs[ii](i,j) - atm_params.airlight_multi_[ii]) / Lsat_horizontal;
        // don't let reflectance value fall below 0
        // value above 1 is unlikely but possible (e.g. specular reflection)
        reflectance(i,j) = vcl_min(vcl_max(0.0f, float(normalized)), 1.0f);
      }
    }
  }
  return true;
}

