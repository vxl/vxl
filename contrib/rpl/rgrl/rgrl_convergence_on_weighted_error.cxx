#include "rgrl_convergence_on_weighted_error.h"

#include <vcl_vector.h>
#include <vcl_cmath.h>

#include "rgrl_match_set.h"
#include "rgrl_set_of.h"
#include "rgrl_converge_status.h"
#include "rgrl_view.h"
#include "rgrl_util.h"

rgrl_convergence_on_weighted_error::
rgrl_convergence_on_weighted_error( double tol )
  : tolerance_( tol )
{
}

rgrl_convergence_on_weighted_error::
~rgrl_convergence_on_weighted_error()
{
}

rgrl_converge_status_sptr
rgrl_convergence_on_weighted_error::
compute_status( rgrl_converge_status_sptr               prev_status,
                rgrl_view                        const& prev_view,
                rgrl_view                        const& current_view,
                rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
                rgrl_set_of<rgrl_scale_sptr>     const& /*current_scales*/,
                bool                                    penalize_scaling) const
{

  // Step2: Take the weighted average of errors as the objective
  //        value.  To avoid the low error given by an infeasible
  //        transformation, the error is scaled by the scaling of
  //        the spread of the transformed data points
  //
  double scaling = 1;
  if ( penalize_scaling ) {
    scaling =  rgrl_util_geometric_error_scaling( current_match_sets );
    DebugMacro(1, "geometric_error_scaling = "<<scaling<<'\n');
  }

  double new_error = scaling * compute_alignment_error( current_match_sets );

  bool good = new_error < tolerance_;

  return compute_status_helper( new_error, good, prev_status, prev_view, current_view );
}

double
rgrl_convergence_on_weighted_error::
compute_alignment_error( rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets ) const
{
  typedef rgrl_match_set::const_from_iterator from_iter;
  typedef from_iter::to_iterator              to_iter;

  // Step1: Compute the errors of all matches. The weights of the
  //        matches are precomputed.
  //
  double error_sum = 0, weight_sum = 0;
  double error, weight;
  for ( unsigned ds=0; ds < current_match_sets.size(); ++ds ) {
    rgrl_match_set const& ms = *current_match_sets[ds];
    for ( from_iter fitr = ms.from_begin(); fitr != ms.from_end(); ++fitr ) {
      //rgrl_feature_sptr mapped = fitr.from_feature()->transform( *current_xform );
      rgrl_feature_sptr mapped = fitr.mapped_from_feature();
      for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {

        error = titr.to_feature()->geometric_error( *mapped );
        weight = titr.cumulative_weight(); //take the precomputed wgt
        
        error_sum += error * weight;
        weight_sum += weight;
      }
    }
  }

  return error_sum/weight_sum;  
}


double
rgrl_convergence_on_weighted_error::
compute_alignment_error( rgrl_match_set_sptr const& current_match_set ) const
{
  assert( current_match_set );
  
  typedef rgrl_match_set::const_from_iterator from_iter;
  typedef from_iter::to_iterator              to_iter;

  // Step1: Compute the errors of all matches. The weights of the
  //        matches are precomputed.
  //
  double error_sum = 0, weight_sum = 0;
  double error, weight;
  rgrl_match_set const& ms = *current_match_set;
  for ( from_iter fitr = ms.from_begin(); fitr != ms.from_end(); ++fitr ) {
    //rgrl_feature_sptr mapped = fitr.from_feature()->transform( *current_xform );
    rgrl_feature_sptr mapped = fitr.mapped_from_feature();
    for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {

      error = titr.to_feature()->geometric_error( *mapped );
      weight = titr.cumulative_weight(); //take the precomputed wgt
      
      error_sum += error * weight;
      weight_sum += weight;
    }
  }

  return error_sum/weight_sum;  
}

rgrl_converge_status_sptr 
rgrl_convergence_on_weighted_error::
  //: verify the final alignment
verify( rgrl_transformation_sptr         const& xform_estimate,
        rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
        rgrl_set_of<rgrl_scale_sptr>     const& current_scales )const
{
  // 
  // this should be penalized by the scaling as well.
  // However, I don't like current implementation that 
  // bool penalize_scaling is passed on every function.
  // I think it should be made a member variable at construction 
  // time.
  // GY

  double error = compute_alignment_error( current_match_sets );
  
  bool good_enough = error < tolerance_;
  rgrl_converge_status_sptr status = new rgrl_converge_status( true, false, good_enough, !good_enough, error, 0, 0 );
  if( good_enough ) {
    status -> set_current_status( rgrl_converge_status::good_and_terminate );
  }
  
  return status;
}                                     
