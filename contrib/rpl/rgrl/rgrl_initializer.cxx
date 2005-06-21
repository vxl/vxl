//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include <rgrl/rgrl_initializer.h>
#include <rgrl/rgrl_view.h>

rgrl_initializer::
~rgrl_initializer()
{
}

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
