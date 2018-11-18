#include "rgrl_initializer.h"
//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_scale.h>

rgrl_initializer::
~rgrl_initializer() = default;

bool
rgrl_initializer::
next_initial( rgrl_mask_sptr           & from_image_roi,
              rgrl_mask_sptr           & to_image_roi,
              rgrl_mask_box            & current_region,
              rgrl_mask_box            & global_region,
              rgrl_estimator_sptr      & xform_estimator,
              rgrl_transformation_sptr & xform_estimate,
              unsigned                 & current_resolution,
              rgrl_scale_sptr          & prior_scale)
{
  rgrl_view_sptr  view;
  bool has_next_view = next_initial( view, prior_scale );

  if (!has_next_view) return has_next_view;

  from_image_roi     = view->from_image_roi();
  to_image_roi       = view->to_image_roi();
  current_region     = view->region();
  global_region      = view->global_region();
  xform_estimator    = view->xform_estimator();
  xform_estimate     = view->xform_estimate();
  current_resolution = view->resolution();
  return has_next_view;
}

bool
rgrl_initializer::
next_initial( rgrl_mask_box            & current_region,
              rgrl_estimator_sptr      & xform_estimator,
              rgrl_transformation_sptr & xform_estimate,
              unsigned                 & current_resolution,
              rgrl_scale_sptr          & prior_scale)
{
  rgrl_view_sptr  view;
  bool has_next_view = next_initial( view, prior_scale );

  if (!has_next_view) return has_next_view;

  current_region     = view->global_region();
  xform_estimator    = view->xform_estimator();
  xform_estimate     = view->xform_estimate();
  current_resolution = view->resolution();
  return has_next_view;
}

bool
rgrl_initializer::
next_initial( rgrl_mask_box            & current_region,
              rgrl_estimator_sptr      & xform_estimator,
              rgrl_transformation_sptr & xform_estimate,
              rgrl_scale_sptr          & prior_scale)
{
  rgrl_view_sptr  view;
  bool has_next_view = next_initial( view, prior_scale );

  if (!has_next_view) return has_next_view;

  current_region     = view->global_region();
  xform_estimator    = view->xform_estimator();
  xform_estimate     = view->xform_estimate();
  return has_next_view;
}

//: force the setting of prior scale
//  scale > 0    set to this scale value
//  scale == 0   scale pointer is NULL, which indicates to use unwgted scale estimate
//  scale < 0    use whatever scale set by the match
void
rgrl_initializer::
set_prior_geometric_scale( double scale )
{
  if ( scale < 0 ) {
    use_prior_scale_from_match_ = true;
  } else if ( scale == 0.0 ) {
    use_prior_scale_from_match_ = false;
    prior_scale_ = nullptr;
  } else {
    use_prior_scale_from_match_ = false;
    prior_scale_ = new rgrl_scale;
    prior_scale_->set_geometric_scale( scale, rgrl_scale::prior );
  }
}

rgrl_scale_sptr
rgrl_initializer::
enforce_prior_scale( const rgrl_scale_sptr& match_scale )
{
  if ( use_prior_scale_from_match_ )
    return match_scale;
  else
    return prior_scale_;   // return the prior scale in this object
}
