#include "rgrl_view.h"
//:
// \file
// \brief  Represent a "view" of the registration problem.
// \author Chuck Stewart
// \date   25 Nov 2002

#include <rgrl/rgrl_estimator.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>

rgrl_view::
rgrl_view()
  : from_image_roi_(0),
    to_image_roi_(0),
    current_region_(0),
    global_region_(0),
    current_resolution_(0)
{
}

rgrl_view::
rgrl_view( rgrl_mask_sptr          const& from_image_roi,
           rgrl_mask_sptr          const& to_image_roi,
           rgrl_mask_box           const& region,
           rgrl_mask_box           const& global_region,
           rgrl_estimator_sptr       xform_estimator,
           rgrl_transformation_sptr  xform_estimate,
           unsigned                  resolution,
           rgrl_transformation_sptr  inverse_estimate )
  : from_image_roi_( from_image_roi ),
    to_image_roi_( to_image_roi ),
    current_region_( region ),
    global_region_( global_region ),
    xform_estimator_( xform_estimator ),
    xform_estimate_( xform_estimate ),
    inverse_estimate_( inverse_estimate ),
    current_resolution_( resolution )
{
  if ( !from_image_roi || !to_image_roi )
  {
    WarningMacro( "ERROR: invalid From/To image ROI.\n       In the simplest case, supply an instance of rgrl_mask_box.\n" );
    assert( 0 ) ;
  }
}

//: return a self copy
rgrl_view_sptr
rgrl_view::
self_copy() const
{
  return new rgrl_view( *this ); 
}

bool
rgrl_view::
is_at_finest_resolution() const
{
  return current_resolution_ == 0;
}

bool
rgrl_view::
current_region_converged() const
{
  return current_region_ == global_region_;
}

bool
rgrl_view::
regions_converged_to(const rgrl_view& other) const
{
  // Check if the x0 and x1 of the current region diff from other region more
  // than one pixel in either x or y direction
  //
  bool current_region_changed =
    ( (this->current_region_.x0() - other.current_region_.x0()).inf_norm() > 1 ||
      (this->current_region_.x1() - other.current_region_.x1()).inf_norm() > 1);
  // Check if the x0 and x1 of the current global region diff from other
  // global region more than one pixel in either x or y direction
  //
  bool current_global_region_changed =
    ( (this->global_region_.x0() - other.global_region_.x0()).inf_norm() > 1 ||
      (this->global_region_.x1() - other.global_region_.x1()).inf_norm() > 1);

  return !current_region_changed &&
         !current_global_region_changed &&
         this->from_image_roi_ == other.from_image_roi_ &&
         this->to_image_roi_ == other.to_image_roi_ &&
         this->xform_estimator_->transformation_type() == other.xform_estimator_->transformation_type() &&
         this->current_resolution_ == other.current_resolution_;
}

bool
rgrl_view::is_valid() const
{
  return xform_estimator_ && xform_estimate_;
}

rgrl_view_sptr
rgrl_view::
scale_by( unsigned new_resol, double scaling ) const
{
  // HACK: to PROPERLY scale image roi
  //       different resolutions of ROI should be provided rather than being computed
  // --GY--
  //assert( dynamic_cast<rgrl_mask_box*>(current_view.from_image_roi().ptr()) );
  //assert( dynamic_cast<rgrl_mask_box*>(current_view.to_image_roi().ptr()) );

  rgrl_mask_sptr from_new_roi, to_new_roi;
  
  if( vcl_abs(scaling-1.0) <= 1e-5 ) { // if scaling is 1.0
    
    from_new_roi = from_image_roi();
    to_new_roi   = to_image_roi();
  
  } else { //approximation
  
    // WarningMacro( "Approximating the region of interest by scale " << scale << vcl_endl );
    from_new_roi = new rgrl_mask_box( from_image_roi()->x0()*scaling,
                                      from_image_roi()->x1()*scaling );
    to_new_roi = new rgrl_mask_box( to_image_roi()->x0()*scaling,
                                    to_image_roi()->x1()*scaling );
  }
  
  rgrl_mask_box new_current_region( region().x0()*scaling,
                                    region().x1()*scaling);
  rgrl_mask_box new_global_region( global_region().x0()*scaling,
                                   global_region().x1()*scaling );

  // forward transformation
  rgrl_transformation_sptr new_xform_estimate;
  if( xform_estimate() )
    new_xform_estimate = xform_estimate()->scale_by( scaling );

  // backward transformation
  rgrl_transformation_sptr new_inv_xform_estimate;
  if( inverse_xform_estimate() )
    new_inv_xform_estimate = inverse_xform_estimate()->scale_by( scaling );

  return new rgrl_view( from_new_roi, to_new_roi, 
                        new_current_region, new_global_region,
                        xform_estimator(),
                        new_xform_estimate,
                        new_resol, 
                        new_inv_xform_estimate);
}

#if 0
bool
rgrl_view::
operator==( const rgrl_view& other ) const
{
  return this->from_image_roi_.x0() == other.from_image_roi_.x0() &&
         this->from_image_roi_.x1() == other.from_image_roi_.x1() &&
         this->to_image_roi_.x0() == other.to_image_roi_.x0() &&
         this->to_image_roi_.x1() == other.to_image_roi_.x1() &&
         this->current_region_.x0() == other.current_region_.x0() &&
         this->current_region_.x1() == other.current_region_.x1() &&
         this->global_region_.x0() == other.global_region_.x0() &&
         this->global_region_.x1() == other.global_region_.x1() &&
         this->xform_estimator_ == other.xform_estimator_ &&
//       this->xform_estimate_ == other.xform_estimate_ &&
         this->current_resolution_ == other.current_resolution_;
}

bool
rgrl_view::
operator!=( const rgrl_view& other ) const
{
  return !( *this == other );
}
#endif // 0
