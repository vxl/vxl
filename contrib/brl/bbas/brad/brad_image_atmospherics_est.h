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

bool brad_estimate_atmospheric_parameters(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters &params);

bool brad_estimate_reflectance_image(vil_image_view<float> const& radiance, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &reflectance);

#endif

