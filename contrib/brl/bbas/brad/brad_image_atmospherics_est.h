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
//   Yi Dong     DEC-2017   introduced new ToA Radiance to ToA Reflectance calcuation
//                          updated ToA Reflectance to Surface Reflectance compuation for all band type
// \endverbatim
//----------------------------------------------------------------------------
#include "brad_image_metadata.h"
#include "brad_atmospheric_parameters.h"
#include <vil/vil_image_view.h>

//: get visible band id from image metadata
bool brad_get_visible_band_id(vil_image_view<float> const& radiance,
                              brad_image_metadata const& mdata,
                              std::vector<unsigned>& vis_band_ids);

//: estimate airlight from dark pixels of the image
bool brad_estimate_airlight(vil_image_view<float> const& radiance,
                            brad_image_metadata const& mdata,
                            std::vector<double>& airlight,
                            bool average_airlight = true);

//: estimate atmospheric parameters without input mean reflectance value
bool brad_estimate_atmospheric_parameters(vil_image_view<float> const& radiance,
                                          brad_image_metadata const& mdata,
                                          brad_atmospheric_parameters &params,
                                          bool average_airlight = true);

//: estimate atmopsheric parameters under a given mean reflectance value (0.15 or 0.3)
//  averge_airlight is the option that compute airlight by averaging all visible bands
bool brad_estimate_atmospheric_parameters(vil_image_view<float> const& radiance,
                                          brad_image_metadata const& mdata,
                                          float mean_reflectance,
                                          brad_atmospheric_parameters &params,
                                          bool constrain_atmospheric_params = true,
                                          bool average_airlight = true);


//: estimate the surface reflectance image from ToA reflectance image
bool brad_estimate_reflectance_image(vil_image_view<float> const& radiance,
                                     brad_image_metadata const& mdata,
                                     float mean_reflectance,
                                     vil_image_view<float>& reflectance,
                                     bool average_airlight = true,
                                     bool is_normalize = true);


//: undo the reflectance -- convert ToA surface reflectance image back to ToA reflectance image
bool brad_undo_reflectance_estimate(vil_image_view<float> const& reflectance,
                                    brad_image_metadata const& mdata,
                                    float mean_reflectance,
                                    vil_image_view<float> &radiance,
                                    bool average_airlight = true,
                                    bool is_normalize = true);

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
