#include "rgrl_convergence_on_weighted_nas.h"

#include <vcl_vector.h>
#include <vcl_cmath.h>

#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_set_of.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_converge_status_nas.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_util.h>
#include <rgrl/rgrl_transformation.h>

rgrl_convergence_on_weighted_nas::
rgrl_convergence_on_weighted_nas( double tol, double scaling_ratio_threshold )
  : rgrl_convergence_on_weighted_error( tol ),
    scaling_ratio_threshold_( scaling_ratio_threshold )
{
}

rgrl_convergence_on_weighted_nas::
~rgrl_convergence_on_weighted_nas()
{
}

rgrl_converge_status_sptr
rgrl_convergence_on_weighted_nas::
compute_status( rgrl_converge_status_sptr               prev_status,
                rgrl_view                        const& prev_view,
                rgrl_view                        const& current_view,
                rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
                rgrl_set_of<rgrl_scale_sptr>     const& current_scales,
                bool                                    penalize_scaling) const
{
  typedef rgrl_match_set::const_from_iterator from_iter;
  typedef from_iter::to_iterator              to_iter;

  rgrl_converge_status_sptr base_status_sptr = 
    rgrl_convergence_on_weighted_error::compute_status( prev_status, 
                                                        prev_view,
                                                        current_view,
                                                        current_match_sets,
                                                        current_scales,
                                                        penalize_scaling );

  rgrl_converge_status_nas const* prev_nas_ptr = 0;
  if( prev_status ) {
    
    prev_nas_ptr = dynamic_cast<rgrl_converge_status_nas const*>( prev_status.as_pointer() );
    
    // assert prev_status is always rgrl_converge_status_nas type
    assert( prev_nas_ptr );
  }
  
  rgrl_converge_status::converge_type converge = base_status_sptr->current_converge();
  rgrl_converge_status::status_type   status   = base_status_sptr->current_status();

  int iter = 0;
  if( prev_nas_ptr ) {
    iter = prev_nas_ptr->iteration() + 1 ;
  }
  
  vnl_vector<double> const& current_scaling = current_view.xform_estimate()->scaling_factors();
  vnl_vector<double> const& init_scaling = prev_nas_ptr->init_scaling_factors();
    
  assert( !init_scaling.size() || current_scaling.size() == init_scaling.size() );

  // start to check failures after 4 iterations

  DebugMacro( 2, "Current scaling factors are: " << current_scaling << vcl_endl );
      
  // compute the difference in scaling factors
  //
  bool failure = false;
  for( unsigned i=0; i<init_scaling.size()&&(!failure); ++i ) {

    double ratio;
    if( current_scaling[i] > init_scaling[i] ) 
      ratio = init_scaling[i] / current_scaling[i];
    else
      ratio = current_scaling[i] / init_scaling[i];
      
    // check
    if( ratio < scaling_ratio_threshold_ ) {
      failure = true;
      DebugMacro( 2, "The ratio " << ratio << " drops below threshold.\n" 
                     << "Current registration is considered failed." << vcl_endl );
    }
  }
    
  // set status
  if( failure )
    status = rgrl_converge_status::failed;
          
  
  rgrl_converge_status_nas* new_status = new rgrl_converge_status_nas( *base_status_sptr );
  new_status->set_iteration( iter );
  new_status->set_current_converge( converge );
  new_status->set_current_status( status );
  new_status->set_init_scaling_factors( init_scaling );
  
  return new_status;
  //return compute_status_helper( new_error, good, prev_status, prev_view, current_view );
}

//: initialize status
rgrl_converge_status_sptr
rgrl_convergence_on_weighted_nas::
init_status( rgrl_view       const& init_view,
             rgrl_scale_sptr const& prior_scale,
             bool                   penalize_scaling ) const
{
  int iter = 0;
  
  rgrl_converge_status_nas* status_ptr 
    = new rgrl_converge_status_nas( false, false, false, false, 1e5, 0, 1e5 );
  
  status_ptr->set_iteration( 0 );
  
  // scaling factors
  vnl_vector<double> const& scaling_factors = init_view.xform_estimate()->scaling_factors();
  if( !scaling_factors.size() ) {
    WarningMacro( "No scaling factors defined in the initial transformation." );
  }
  
  DebugMacro( 2, "Initialize scaling factors to " << scaling_factors << vcl_endl );    
  status_ptr->set_init_scaling_factors( scaling_factors );
  
  return status_ptr;
}
 