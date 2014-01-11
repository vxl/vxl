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
//----------------------------------------------------------------------------
#include "brad_image_metadata.h"
#include "brad_atmospheric_parameters.h"
#include <vil/vil_image_view.h>

bool brad_estimate_atmospheric_parameters(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters &params);
bool brad_estimate_atmospheric_parameters(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, float mean_reflectance, brad_atmospheric_parameters &params, bool constrain_atmospheric_params = true);

// compute the params from a multi band image
bool brad_estimate_atmospheric_parameters_multi(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters &params);
bool brad_estimate_atmospheric_parameters_multi(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, float mean_reflectance, brad_atmospheric_parameters &params, bool constrain_atmospheric_params = true);

bool brad_estimate_reflectance_image(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &reflectance);
bool brad_estimate_reflectance_image_multi(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &reflectance);
bool brad_undo_reflectance_estimate(vil_image_view<float> const& reflectance, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &radiance);

#endif

