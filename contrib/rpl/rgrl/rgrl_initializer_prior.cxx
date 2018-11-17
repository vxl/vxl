//:
// \file
// \author Chuck Stewart

#include "rgrl_initializer_prior.h"
#include "rgrl_view.h"
#include "rgrl_transformation.h"

rgrl_initializer_prior::
rgrl_initializer_prior( const rgrl_view_sptr&           prior_view,
                        const rgrl_scale_sptr&          prior_scale )
  : init_view_( prior_view ),
    prior_scale_( prior_scale ),
    xform_index_( 0 )
{
  views_.push_back( prior_view );
}

rgrl_initializer_prior::
rgrl_initializer_prior( rgrl_mask_sptr             const& from_image_roi,
                        rgrl_mask_sptr             const& to_image_roi,
                        rgrl_mask_box              const& initial_from_image_roi,
                        const rgrl_estimator_sptr&        xform_estimator,
                        const rgrl_transformation_sptr&   xform_estimate,
                        unsigned                   initial_resolution,
                        const rgrl_scale_sptr&            prior_scale )
  : prior_scale_( prior_scale ),
    xform_index_( 0 )
{
  rgrl_mask_box global_region( from_image_roi->bounding_box() );
  init_view_ = new rgrl_view( from_image_roi, to_image_roi,
                              initial_from_image_roi,
                              global_region, xform_estimator,
                              xform_estimate,
                              initial_resolution );
  views_.push_back( init_view_ );
}

rgrl_initializer_prior::
rgrl_initializer_prior( rgrl_mask_sptr             const& from_image_roi,
                        rgrl_mask_sptr             const& to_image_roi,
                        const rgrl_estimator_sptr&        xform_estimator,
                        const rgrl_transformation_sptr&   xform_estimate,
                        unsigned                   initial_resolution,
                        const rgrl_scale_sptr&            prior_scale )
  : prior_scale_( prior_scale ),
    xform_index_( 0 )
{
  rgrl_mask_box global_region( from_image_roi->bounding_box() );
  init_view_ = new rgrl_view( from_image_roi, to_image_roi,
                              global_region, global_region,
                              xform_estimator, xform_estimate,
                              initial_resolution );
  views_.push_back( init_view_ );
}

rgrl_initializer_prior::
rgrl_initializer_prior( rgrl_mask_sptr             const& from_image_roi,
                        const rgrl_estimator_sptr&        xform_estimator,
                        const rgrl_transformation_sptr&   xform_estimate,
                        unsigned                   initial_resolution,
                        const rgrl_scale_sptr&            prior_scale )
  : prior_scale_( prior_scale ),
    xform_index_( 0 )
{
  rgrl_mask_box global_region( from_image_roi->bounding_box() );
  init_view_ = new rgrl_view( from_image_roi, from_image_roi,
                              global_region, global_region, xform_estimator, xform_estimate,
                              initial_resolution );
  views_.push_back( init_view_ );
}

void
rgrl_initializer_prior::add_prior_xform( const rgrl_transformation_sptr&   xform_estimate )
{

  rgrl_view_sptr view = new rgrl_view( init_view_->from_image_roi(),
                        init_view_->to_image_roi(),
                        init_view_->region(),
                        init_view_->global_region(),
                        init_view_->xform_estimator(),
                        xform_estimate,
                        init_view_->resolution() );
  views_.push_back( view );
}

bool
rgrl_initializer_prior::
next_initial( rgrl_view_sptr           & view,
              rgrl_scale_sptr          & prior_scale )
{
  if ( xform_index_ >= views_.size())
    return false;

  view = views_[xform_index_];
  prior_scale = this->prior_scale_;
  xform_index_++;

  return true;
}
