//:
// \file
// \author Chuck Stewart

#include "rgrl_initializer_prior.h"
#include "rgrl_view.h"

rgrl_initializer_prior::
rgrl_initializer_prior( rgrl_view_sptr           prior_view,
                        rgrl_scale_sptr          prior_scale )
  : init_view_( prior_view ),
    prior_scale_( prior_scale ),
    called_before_( false )
{
}

rgrl_initializer_prior::
rgrl_initializer_prior( rgrl_mask_box              const& from_image_roi,
                        rgrl_mask_box              const& to_image_roi,
                        rgrl_mask_box              const& initial_from_image_roi,
                        rgrl_estimator_sptr        xform_estimator,
                        rgrl_transformation_sptr   xform_estimate,
                        unsigned                   initial_resolution,
                        rgrl_scale_sptr            prior_scale )
  : prior_scale_( prior_scale ),
    called_before_( false )
{
  init_view_ = new rgrl_view( from_image_roi, to_image_roi, 
                              initial_from_image_roi, 
                              from_image_roi, xform_estimator, 
                              xform_estimate, 
                              initial_resolution );
}

rgrl_initializer_prior::
rgrl_initializer_prior( rgrl_mask_box              const& from_image_roi,
                        rgrl_mask_box              const& to_image_roi,
                        rgrl_estimator_sptr        xform_estimator,
                        rgrl_transformation_sptr   xform_estimate,
                        unsigned                   initial_resolution,
                        rgrl_scale_sptr            prior_scale )
  : prior_scale_( prior_scale ),
    called_before_( false )
{
  init_view_ = new rgrl_view( from_image_roi, to_image_roi, from_image_roi, 
                              from_image_roi, xform_estimator, xform_estimate, 
                              initial_resolution );
}

rgrl_initializer_prior::
rgrl_initializer_prior( rgrl_mask_box              const& from_image_roi,
                        rgrl_estimator_sptr        xform_estimator,
                        rgrl_transformation_sptr   xform_estimate,
                        unsigned                   initial_resolution,
                        rgrl_scale_sptr            prior_scale )
  : prior_scale_( prior_scale ),
    called_before_( false )
{
  init_view_ = new rgrl_view( from_image_roi, from_image_roi, from_image_roi, 
                              from_image_roi, xform_estimator, xform_estimate, 
                              initial_resolution );
}

bool
rgrl_initializer_prior::
next_initial( rgrl_view_sptr           & view,
              rgrl_scale_sptr          & prior_scale )
{
  if( called_before_ ) 
    return false;

  view = this->init_view_;
  prior_scale = this->prior_scale_;
  this->called_before_ = true;

  return true;
}
