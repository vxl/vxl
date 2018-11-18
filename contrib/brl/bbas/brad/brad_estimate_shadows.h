//-----------------------------------------------------------------------------
//:
// \file
// \brief A collection of functions for estimating pixels in shadow in an image
// \author D. E. Crispell
// \date February 17, 2012
//
//----------------------------------------------------------------------------
#ifndef brad_estimate_shadows_h_
#define brad_estimate_shadows_h_

#include <vil/vil_image_view.h>
#include "brad_image_metadata.h"
#include "brad_atmospheric_parameters.h"

bool brad_estimate_shadow_prob_density(vil_image_view<float> const& radiance_image, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &shadow_prob_density);

bool brad_estimate_shadow_prob(vil_image_view<float> const& radiance_image, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &shadow_prob);


#endif
