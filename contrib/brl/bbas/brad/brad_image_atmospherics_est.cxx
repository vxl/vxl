#include <sstream>
#include <stdexcept>

#include "vil/vil_copy.h"
#include "vil/vil_math.h"
#include "vnl/vnl_math.h"
#include <bsta/bsta_histogram.h>
#include "brad_image_metadata.h"
#include "brad_utils.h"

#include "brad_image_atmospherics_est.h"


//: get visible band ids from image metadata
std::vector<size_t>
brad_visible_band_ids(brad_image_metadata const& mdata)
{
  auto nbands = mdata.n_bands_;

  // 1 band - immediate return
  if (nbands == 1) {
    return {0};
  }

  // 4-bands & recognized satellite
  else if (nbands == 4) {
    if (mdata.satellite_name_ == "GeoEye-1" ||
        mdata.satellite_name_ == "QuickBird" ||
        mdata.satellite_name_ == "IKONOS") {
      return {0,1,2};
    }
  }

  // 8-bands, multispectral, recognized satellite
  else if (nbands == 8) {
    if (mdata.band_ == "MULTI") {
      if (mdata.satellite_name_ == "WorldView2" ||
          mdata.satellite_name_ == "WorldView3") {
        return {1,2,3,4};
      }
    }
  }

  // 16 bands, recognized satellite
  else if (nbands == 16) {
    if (mdata.satellite_name_ == "WorldView3") {
      return {1,2,3,4};
    }
  }

  // default - use first band
  return {0};
}


// estimate airlight from dark pixels of the ToA reflectance image
// - produces a vector of airlight values, one per band of the input image
// - if no visible bands are specified or a single band input, airlight is
//   determined independently for each band
// - if multiple visible bands are specified, only those bands are used to
//   determine airlight pixels
std::vector<double>
brad_airlight(vil_image_view<float> const& toa_reflectance,
              std::vector<size_t> vis_band_ids,
              double frac)
{
  // image attributes
  size_t ni = toa_reflectance.ni();
  size_t nj = toa_reflectance.nj();
  size_t np = toa_reflectance.nplanes();

  // validate vis_band_ids
  brad_utils::validate_band_ids(np, vis_band_ids);

  // output setup
  std::vector<double> airlight(np, 0.0);

  // compute airlight for each band independently
  if (vis_band_ids.empty()) {

    for (size_t p = 0; p < np; p++) {
      auto band = vil_plane(toa_reflectance, p);

      // histogram of visible pixels
      float minval, maxval;
      vil_math_value_range(band, minval, maxval);

      bsta_histogram<double> h(minval, maxval, 512);
      for (size_t j = 0; j < nj; j++) {
        for (size_t i = 0; i < ni; i++) {
          h.upcount(band(i,j), 1.0f);
        }
      }

      // threshold identifying dark pixels
      double airlight_thresh = h.value_with_area_below(frac);
      airlight[p] = airlight_thresh;
    }

  }

  // compute airlight from visible bands
  else {

    // "visible" image - sum pixel values across visible bands
    vil_image_view<float> vis_image(ni, nj);
    vis_image.fill(0.0f);

    for (auto vis_band_id : vis_band_ids) {
      vil_image_view<float> band = vil_plane(toa_reflectance, vis_band_id);
      for (size_t j = 0; j < nj; j++) {
        for (size_t i = 0; i < ni; i++) {
          vis_image(i,j) += band(i, j);
        }
      }
    }

    // histogram of visible pixels
    float minval, maxval;
    vil_math_value_range(vis_image, minval, maxval);

    bsta_histogram<double> h(minval, maxval, 512);
    for (size_t j = 0; j < nj; j++) {
      for (size_t i = 0; i < ni; i++) {
        h.upcount(vis_image(i,j), 1.0f);
      }
    }

    // threshold identifying dark pixels
    double vis_image_thresh = h.value_with_area_below(frac);

    // calculate airlight threshold for each band
    // average intensity for "airlight" pixels
    for (size_t p = 0; p < np; p++) {
      auto band = vil_plane(toa_reflectance, p);

      double sum = 0.0;
      size_t count = 0;
      for (size_t j = 0; j < nj; j++) {
        for (size_t i = 0; i < ni; i++) {
          if (vis_image(i, j) < vis_image_thresh) {
            sum += double(band(i, j));
            count++;
          }
        }
      }
      double airlight_thresh = sum / double(count);
      airlight[p] = airlight_thresh;
    }
  }

  // cleanup
  return airlight;
}


// estimate average intensity from ToA reflectance
// - produces a vector of average values, one per band of the input image
// - if no visible bands are specified or a single band input, output is
//   determined independently for each band
// - if multiple visible bands are specified, only those bands are used to
//   determine relectance mean for all bands
std::vector<double>
brad_toa_reflectance_mean(vil_image_view<float> const& toa_reflectance,
                          std::vector<size_t> vis_band_ids)
{
  // image attributes
  size_t ni = toa_reflectance.ni();
  size_t nj = toa_reflectance.nj();
  size_t np = toa_reflectance.nplanes();

  // validate vis_band_ids
  brad_utils::validate_band_ids(np, vis_band_ids);

  // compute airlight for each band independently
  if (vis_band_ids.empty()) {

    std::vector<double> output(np, 0.0);
    for (size_t p = 0; p < np; p++) {
      double band_mean = 0.0;
      vil_math_mean(band_mean, toa_reflectance, p);
      output[p] = band_mean;
    }

    return output;

  }

  // compute from visible bands - single "mean" for all bands
  else {

    double sum = 0.0;
    for (auto vis_band_id : vis_band_ids) {
      double band_sum = 0.0;
      vil_math_sum(band_sum, toa_reflectance, vis_band_id);
      sum += band_sum;
    }
    double mean = sum / double(ni * nj * vis_band_ids.size());

    return std::vector<double>(np, mean);
  }

}


// estimate surface reflectance from ToA reflectance & statistics
// surf_refl = surf_refl_mean * (toa_refl - airlight) /
//                              (toa_refl_mean - airlight)
vil_image_view<float>
brad_surface_reflectance(vil_image_view<float> const& toa_reflectance,
                         std::vector<double> toa_reflectance_mean,
                         std::vector<double> airlight,
                         double surface_reflectance_mean)
{
  // image attributes
  size_t ni = toa_reflectance.ni();
  size_t nj = toa_reflectance.nj();
  size_t np = toa_reflectance.nplanes();

  // validate inputs
  brad_utils::validate_vector(np, toa_reflectance_mean, "ToA reflectance mean");
  brad_utils::validate_vector(np, airlight, "airlight", true);

  // initialize output
  auto surface_reflectance = vil_copy_deep(toa_reflectance);

  // apply image correction
  for (size_t p = 0; p < np; p++) {
    double toa_mean_p = toa_reflectance_mean[p];
    double airlight_p = airlight.empty() ? 0.0 : airlight[p];

    double scale = surface_reflectance_mean / (toa_mean_p - airlight_p);
    double offset = - scale * airlight_p;

    auto band = vil_plane(surface_reflectance, p);
    vil_math_scale_and_offset_values(band, scale, offset);
  }

  // cleanup
  return surface_reflectance;
}




bool brad_get_visible_band_id(vil_image_view<float> const& radiance,
                              brad_image_metadata const& mdata,
                              std::vector<unsigned>& vis_band_ids)
{
  unsigned int np = radiance.nplanes();
  if (np == 1)
    vis_band_ids.push_back(0);
  else if (np == 4) {
    if (mdata.satellite_name_ == "GeoEye-1" || mdata.satellite_name_ == "QuickBird" || mdata.satellite_name_ == "IKONOS") {
      vis_band_ids.push_back(0);
      vis_band_ids.push_back(1);
      vis_band_ids.push_back(2);
    }
    else
      vis_band_ids.push_back(0);
  }
  else if (np == 8) {
    if (mdata.band_ == "MULTI")
      if (mdata.satellite_name_ == "WorldView2" || mdata.satellite_name_ == "WorldView3") {
        vis_band_ids.push_back(1);
        vis_band_ids.push_back(2);
        vis_band_ids.push_back(3);
        vis_band_ids.push_back(4);
      }
      else vis_band_ids.push_back(0);
    else vis_band_ids.push_back(0);  // for WorldView3 SWIR 8 Band images
  }
  else if (np == 16 && mdata.satellite_name_ == "WorldView3")  // for WorldView3 MULTI+SWIR case
  {
    vis_band_ids.push_back(1);
    vis_band_ids.push_back(2);
    vis_band_ids.push_back(3);
    vis_band_ids.push_back(4);
  }
  else {
    vis_band_ids.push_back(0); // use first band for airlight estimation
  }
  return true;
}

bool brad_estimate_airlight(vil_image_view<float> const& radiance,
                            brad_image_metadata const& mdata,
                            std::vector<double>& out_airlight,
                            bool average_airlight)
{
  float frac = 0.001f;
  unsigned int ni = radiance.ni();
  unsigned int nj = radiance.nj();
  unsigned int np = radiance.nplanes();
  if (average_airlight) {
    // get visible band based on input satellite type and band type
    std::vector<unsigned> vis_bands;
    brad_get_visible_band_id(radiance, mdata, vis_bands);
    // compute average image pixel values
    vil_image_view<float> avg_img;
    avg_img.set_size(ni, nj);
    avg_img.fill(0.0f);
    for (unsigned int & vis_band : vis_bands)
      for (size_t j = 0; j < nj; j++)
        for (size_t i = 0; i < ni; i++) {
          const float pixel_val = radiance(i, j, vis_band);
          avg_img(i,j) += pixel_val;
        }
    float minval, maxval;
    vil_math_value_range(avg_img, minval, maxval);

    bsta_histogram<double> h(minval, maxval, 512);
    for (unsigned j = 0; j < nj; j++)
      for (unsigned i = 0; i < ni; i++)
        h.upcount(avg_img(i,j), 1.0f);
    double airlight_avg = h.value_with_area_below(frac);
    // calculate atmospheric parameters for each band
    std::vector<double> airlight;
    airlight.resize(np);
    for (unsigned p = 0; p < np; p++)
    {
      vil_image_view<float> img = vil_plane(radiance, p);
      double airlight_sum = 0.0;
      int count = 0;
      //compute airlight for this plane
      for (size_t j = 0; j < nj; j++) {
        for (size_t i = 0; i < ni; i++) {
          if (avg_img(i, j) < airlight_avg) {
            airlight_sum += (double)img(i, j);
            count++;
          }
        }
      }
      airlight[p] = (double)(airlight_sum / count);
    }
    out_airlight = airlight;
  }
  else {
    for (unsigned p = 0; p < np; p++) {
      vil_image_view<float> band = vil_plane(radiance, p);
      float minval, maxval;
      vil_math_value_range(band, minval, maxval);
      bsta_histogram<double> h(minval, maxval);
      for (unsigned j = 0; j < nj; j++)
        for (unsigned i = 0; i < ni; i++)
          h.upcount(band(i,j), 1.0f);
      double airlight = h.value_with_area_below(frac);
      out_airlight.push_back(airlight);
    }
  }
  return true;
}

bool brad_estimate_atmospheric_parameters(vil_image_view<float> const& radiance,
                                          brad_image_metadata const& mdata,
                                          brad_atmospheric_parameters &params,
                                          bool average_airlight)
{
  unsigned np = radiance.nplanes();
  std::vector<double> airlight;
  brad_estimate_airlight(radiance, mdata, airlight, average_airlight);
  params.airlight_multi_ = airlight;
  params.airlight_ = params.airlight_multi_[0];

  // put fix values for skylight and optical_depth
  for (unsigned i = 0; i < np; i++) {
    params.optical_depth_multi_.push_back(0.0);
    params.skylight_multi_.push_back(0.0);
  }
  params.skylight_ = params.skylight_multi_[0];
  params.optical_depth_ = params.optical_depth_multi_[0];
  return true;
}

bool brad_estimate_atmospheric_parameters(vil_image_view<float> const& radiance,
                                          brad_image_metadata const& mdata,
                                          float mean_reflectance,
                                          brad_atmospheric_parameters &params,
                                          bool constrain_atmospheric_params,
                                          bool average_airlight)
{
  unsigned np = radiance.nplanes();
  // get sun irradiance values
  std::string band_type = mdata.band_;
  std::vector<double> sun_irradiance_vals = mdata.normal_sun_irradiance_values_;

  if (sun_irradiance_vals.size() != np) {
    std::cerr << "brad_image_atmospherics_est::brad_estimate_atmospheric_parameters: Mismatch of sun irradiance " << sun_irradiance_vals.size()
              << " and image band number " << np << "!!!\n";
    return false;
  }

  // calculate airlight
  std::vector<double> airlight;
  brad_estimate_airlight(radiance, mdata, airlight, average_airlight);
  params.airlight_multi_ = airlight;
  params.airlight_ = params.airlight_multi_[0];

  // compute optical depth
  for (unsigned p = 0; p < np; p++)
  {
    vil_image_view<float> img = vil_plane(radiance, p);
    // airlight
    double airlight = params.airlight_multi_[p];
    // find image mean
    double radiance_mean;
    vil_math_mean(radiance_mean, img, 0);
    // check image mean value and computed airlight
    if (radiance_mean < airlight) {
      std::cerr << "brad_image_atmospherics_est::brad_estimate_atmospheric_parameters: radiance_mean " << radiance_mean
                << " is less than evaluated airlight " << airlight << " at band " << p << "!!!\n";
      return false;
    }
    // calculate optical depth
    double deg2rad = vnl_math::pi_over_180;
    double numerator = vnl_math::pi * (radiance_mean - airlight);
    double denominator = mean_reflectance * sun_irradiance_vals[p] * std::sin(deg2rad * mdata.sun_elevation_);
    double optical_depth = -std::log(numerator / denominator);
    optical_depth /= (1.0 / std::sin(deg2rad*mdata.view_elevation_) + 1.0 / std::sin(deg2rad*mdata.sun_elevation_));

    if (constrain_atmospheric_params) {
      // Optical depth cannot be less than 0
      // in practice, we may not have reliable metadata, in which case the best we can hope for is
      // a reasonable normalization of image intensities - optical depth may need to be < 0 in this case.
      optical_depth = std::max(0.0, optical_depth);
    }
    params.optical_depth_multi_.push_back(optical_depth);
  }
  // set fix skylight value
  for (unsigned p = 0; p < np; p++)
    params.skylight_multi_.push_back(0.0);
  params.optical_depth_ = params.optical_depth_multi_[0];
  params.skylight_ = params.skylight_multi_[0];
  return true;
}

// estimate the surface reflectance image from ToA reflectance image
bool brad_estimate_reflectance_image(vil_image_view<float> const& radiance,
                                     brad_image_metadata const& mdata,
                                     float mean_reflectance,
                                     vil_image_view<float>& reflectance,
                                     bool average_airlight,
                                     bool is_normalize)
{
  unsigned ni = radiance.ni();
  unsigned nj = radiance.nj();
  unsigned np = radiance.nplanes();
  // compute airlight
  std::vector<double> airlight;
  brad_estimate_airlight(radiance, mdata, airlight, average_airlight);

  // obtain visible band ids
  std::vector<unsigned> band_ids;
  brad_get_visible_band_id(radiance, mdata, band_ids);

  reflectance.set_size(ni, nj, np);
  reflectance.fill(0.0f);
  // apply image correction
  for (unsigned p = 0; p < np; p++) {
    for (unsigned j = 0; j < nj; j++) {
      for (unsigned i = 0; i < ni; i++) {
        float ref = radiance(i,j,p) - (float)airlight[p];
        reflectance(i,j,p) = std::max(0.0f, ref);
      }
    }
  }
  if (!is_normalize)
    return true;

  // normalize by average radiance
  double Lsat_horizontal = 0.0;
  for (unsigned int p : band_ids)
  {
    vil_image_view<float> cur_plane = vil_plane(radiance, p);
    double plane_ave = 0.0;
    for (unsigned j = 0; j < cur_plane.nj(); j++)
      for (unsigned i = 0; i < cur_plane.ni(); i++)
        plane_ave += double(cur_plane(i,j));
    plane_ave /= cur_plane.ni() * cur_plane.nj();
    Lsat_horizontal += (plane_ave - airlight[p]);
  }
  Lsat_horizontal /= band_ids.size();
  auto norm_factor = (float)(mean_reflectance / Lsat_horizontal);

  // apply normalization
  for (unsigned p = 0; p < np; p++) {
    for (unsigned j = 0; j < nj; j++) {
      for (unsigned i = 0; i < ni; i++) {
        reflectance(i,j,p) *= norm_factor;
      }
    }
  }

  return true;
}

bool brad_undo_reflectance_estimate(vil_image_view<float> const& reflectance,
                                    brad_image_metadata const& mdata,
                                    float mean_reflectance,
                                    vil_image_view<float> &radiance,
                                    bool average_airlight,
                                    bool is_normalize)
{
  // compuate airlight
  unsigned ni = reflectance.ni();
  unsigned nj = reflectance.nj();
  unsigned np = reflectance.nplanes();

  radiance.set_size(ni, nj, np);
  radiance.fill(0.0f);
  // compuate airlight
  std::vector<double> airlight;
  brad_estimate_airlight(radiance, mdata, airlight, average_airlight);

  if (is_normalize)
  {
    // compuate normalization factor
    // obtain visible band ids
    std::vector<unsigned> band_ids;
    brad_get_visible_band_id(radiance, mdata, band_ids);
    double Lsat_horizontal = 0.0;
    for (unsigned int p : band_ids)
    {
      vil_image_view<float> cur_plane = vil_plane(radiance, p);
      double plane_ave = 0.0;
      for (unsigned j = 0; j < cur_plane.nj(); j++)
        for (unsigned i = 0; i < cur_plane.ni(); i++)
          plane_ave += double(cur_plane(i,j));
      plane_ave /= cur_plane.ni() * cur_plane.nj();
      Lsat_horizontal += (plane_ave - airlight[p]);
    }
    Lsat_horizontal /= band_ids.size();
    auto norm_factor = (float)(Lsat_horizontal / mean_reflectance);
    // apply inverse
    for (unsigned p = 0; p < np; p++) {
      for (unsigned j = 0; j < nj; j++) {
        for (unsigned i = 0; i < ni; i++) {
          radiance(i,j,p) = reflectance(i,j,p) * norm_factor + (float)airlight[p];
        }
      }
    }
  }
  else
  {
    for (unsigned p = 0; p < np; p++) {
      for (unsigned j = 0; j < nj; j++) {
        for (unsigned i = 0; i < ni; i++) {
          radiance(i,j,p) = reflectance(i,j,p) + (float)airlight[p];
        }
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
  for (size_t j = 0; j < nj; j++) {
    for (size_t i = 0; i < ni; i++) {
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
    for (size_t j = 0; j < nj; j++) {
      for (size_t i = 0; i < ni; i++) {
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
    for (size_t j = 0; j < cur_plane.nj(); j++) {
      for (size_t i = 0; i < cur_plane.ni(); i++) {
        plane_ave += double(cur_plane(i, j));
      }
    }
    plane_ave /= cur_plane.ni()*cur_plane.nj();
    Lsat_horizontal += (plane_ave - airlight[p]);
  }
  Lsat_horizontal /= (max_norm_band - min_norm_band + 1);

  auto norm_factor = (float)(mean_reflectance / Lsat_horizontal);

  cal_img.set_size(ni, nj, np);
  cal_img.fill(0.0);
  // apply image correction
  for (size_t p = 0; p < radiance.nplanes(); p++) {
    for (size_t j = 0; j < radiance.nj(); j++) {
      for (size_t i = 0; i < radiance.ni(); i++) {
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
  // get sun irradiance values
  std::string band_type = mdata.band_;
  std::vector<double> sun_irradiance_vals = mdata.normal_sun_irradiance_values_;

  if (sun_irradiance_vals.size() != radiance.nplanes()) {
    std::cerr << "brad_image_atmospherics_est::brad_estimate_atmospheric_parameters: Mismatch of sun irradiance " << sun_irradiance_vals.size()
              << " and image band number " << radiance.nplanes() << "!!!\n";
    return false;
  }

  if (radiance.nplanes() != sun_irradiance_vals.size()) {
    std::cerr << "brad_image_atmospherics_est::brad_atmo_radiance_to_reflectance: "
              << "Mismatch of image plane numebr to the length of solar irradiance.  "
              << "Image plane number: " << radiance.nplanes()
              << ", solar irradiance value length: " << sun_irradiance_vals.size() << "!!!\n";
    return false;
  }

  double sun_dot_norm = std::sin(mdata.sun_elevation_*vnl_math::pi_over_180);

  for (unsigned b = 0; b < radiance.nplanes(); b++){

    double band_norm = 1.0/(sun_irradiance_vals[b] * sun_dot_norm / vnl_math::pi);

    vil_image_view<float> rad_band = vil_plane(radiance, b);
    vil_image_view<float> ref_band = vil_plane(reflectance, b);
    ref_band.deep_copy(rad_band);
    vil_math_scale_values(ref_band, band_norm);
  }
  return true;

}
