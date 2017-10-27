#ifndef brad_image_atmospherics_est_h_
#define brad_image_atmospherics_est_h_

//-----------------------------------------------------------------------------
//:
// \file
// \brief Functions for estimating atmospheric parameters from image data and metadata
//
//
// \author D. E. Crispell
// \date January 28, 2012
//
//  Modifications
//   Yi Dong     OCT-2014   added support for eight band images
//   Tom Pollard APRIL-2017 improved support for eight band images
// \endverbatim
//----------------------------------------------------------------------------
#include "brad_image_metadata.h"
#include "brad_atmospheric_parameters.h"
#include <vil/vil_image_view.h>

bool brad_estimate_atmospheric_parameters(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters &params);
bool brad_estimate_atmospheric_parameters(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, float mean_reflectance, brad_atmospheric_parameters &params, bool constrain_atmospheric_params = true);

// compute the params from a multi band image
bool brad_estimate_atmospheric_parameters_multi(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters &params);
bool brad_estimate_atmospheric_parameters_multi(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, float mean_reflectance, brad_atmospheric_parameters &params, bool constrain_atmospheric_params = true);
bool brad_estimate_atmospheric_parameters_multi_average(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, float mean_reflectance, brad_atmospheric_parameters &params, int dark_pixels_band, bool constrain_atmospheric_params = true);

// estimate the reflectance image
bool brad_estimate_reflectance_image(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &reflectance);
bool brad_estimate_reflectance_image_multi(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &reflectance);
bool brad_undo_reflectance_estimate(vil_image_view<float> const& reflectance, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &radiance);

// This function is equivalent to calling both:
//   brad_estimate_atmospheric_parameters_multi
//   brad_estimate_reflectance_image_multi
// It was discovered that the result of calling these functions in sequence is
// independent of the metadata and can be simplified.  In future development
// this function should be called instead of the above.
bool brad_estimate_reflectance_image_multi(
  vil_image_view<float> const& radiance, // image 
  float mean_reflectance,                // average albedo
  vil_image_view<float> & cal_img,       // corrected image
  int min_norm_band = -1,                // minimum band # that will be used for determining dark pixels and normalization factor 
  int max_norm_band = -1                 // maximum band # that will be used for determining dark pixels and normalization factor 
);

// Convert from top-of-atmosphere radiance to top-of-atmosphere reflectance
bool brad_atmo_radiance_to_reflectance(
  vil_image_view<float> const& radiance,
  brad_image_metadata const& mdata, 
  vil_image_view<float> &reflectance);

#endif

