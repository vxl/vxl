//:
// \file
// \author Chuck Stewart

#include "rgrl_initializer_prior.h"
#include "rgrl_view.h"
#include "rgrl_transformation.h"

rgrl_initializer_prior::
rgrl_initializer_prior( rgrl_view_sptr           prior_view,
                        rgrl_scale_sptr          prior_scale )
  : init_view_( prior_view ),
    prior_scale_( prior_scale ),
    xform_index_( 0 )
{
  xforms_.push_back( init_view_->xform_estimate() );
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
    xform_index_( 0 )
{
  init_view_ = new rgrl_view( from_image_roi, to_image_roi,
                              initial_from_image_roi,
                              from_image_roi, xform_estimator,
                              xform_estimate,
                              initial_resolution );
  xforms_.push_back( xform_estimate );
}

rgrl_initializer_prior::
rgrl_initializer_prior( rgrl_mask_box              const& from_image_roi,
                        rgrl_mask_box              const& to_image_roi,
                        rgrl_estimator_sptr        xform_estimator,
                        rgrl_transformation_sptr   xform_estimate,
                        unsigned                   initial_resolution,
                        rgrl_scale_sptr            prior_scale )
  : prior_scale_( prior_scale ),
    xform_index_( 0 )
{
  init_view_ = new rgrl_view( from_image_roi, to_image_roi, from_image_roi,
                              from_image_roi, xform_estimator, xform_estimate,
                              initial_resolution );
  xforms_.push_back( xform_estimate );
}

rgrl_initializer_prior::
rgrl_initializer_prior( rgrl_mask_box              const& from_image_roi,
                        rgrl_estimator_sptr        xform_estimator,
                        rgrl_transformation_sptr   xform_estimate,
                        unsigned                   initial_resolution,
                        rgrl_scale_sptr            prior_scale )
  : prior_scale_( prior_scale ),
    xform_index_( 0 )
{
  init_view_ = new rgrl_view( from_image_roi, from_image_roi, from_image_roi,
                              from_image_roi, xform_estimator, xform_estimate,
                              initial_resolution );
  xforms_.push_back( xform_estimate );
}

void
rgrl_initializer_prior::add_prior_xform( rgrl_transformation_sptr   xform_estimate )
{
  xforms_.push_back( xform_estimate );
}

bool
rgrl_initializer_prior::
next_initial( rgrl_view_sptr           & view,
              rgrl_scale_sptr          & prior_scale )
{
  if ( xform_index_ >= xforms_.size())
    return false;

  view = new rgrl_view( init_view_->from_image_roi(),
                        init_view_->to_image_roi(),
                        init_view_->region(),
                        init_view_->global_region(),
                        init_view_->xform_estimator(),
                        xforms_[xform_index_],
                        init_view_->resolution() );
  prior_scale = this->prior_scale_;
  xform_index_++;

  return true;
}
