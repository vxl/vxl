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


rgrl_converge_status_sptr
rgrl_convergence_on_weighted_error::
compute_status( rgrl_converge_status_sptr               prev_status,
                rgrl_view                        const& prev_view,
                rgrl_view                        const& current_view,
                rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
                rgrl_set_of<rgrl_scale_sptr>     const& /*current_scales*/,
                bool                                    penalize_scaling) const
{
  typedef rgrl_match_set::const_from_iterator from_iter;
  typedef from_iter::to_iterator              to_iter;
  typedef vcl_vector<double>::iterator        vec_iter;

  // Step1: Compute the errors of all matches. The weights of the
  //        matches are precomputed.
  //
  //rgrl_transformation_sptr current_xform = current_view.xform_estimate();
  vcl_vector<double> errors;
  vcl_vector<double> weights;

  for ( unsigned ds=0; ds < current_match_sets.size(); ++ds ) {
    rgrl_match_set const& ms = *current_match_sets[ds];
    for ( from_iter fitr = ms.from_begin(); fitr != ms.from_end(); ++fitr ) {
      //rgrl_feature_sptr mapped = fitr.from_feature()->transform( *current_xform );
      rgrl_feature_sptr mapped = fitr.mapped_from_feature();
      for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
        errors.push_back( titr.to_feature()->geometric_error( *mapped ) );
        weights.push_back( titr.cumulative_weight() ); //take the precomputed wgt
      }
    }
  }

  // Step2: Take the weighted average of errors as the objective
  //        value.  To avoid the low error given by an infeasible
  //        transformation, the error is scaled by the scaling of
  //        the spread of the transformed data points
  //
  vec_iter eitr = errors.begin();
  vec_iter witr = weights.begin();
  double error_sum = 0, weight_sum = 0;
  for ( ; eitr!=errors.end(); ++eitr, ++witr ) {
    error_sum += (*eitr) * (*witr);
    weight_sum +=  (*witr);
  }

  double scaling = 1;
  if ( penalize_scaling ) {
    scaling =  rgrl_util_geometric_error_scaling( current_match_sets );
    DebugMacro(1, "geometric_error_scaling = "<<scaling<<'\n');
  }

  double new_error = error_sum/weight_sum * scaling;

  bool good = new_error < tolerance_;

  return compute_status_helper( new_error, good, prev_status, prev_view, current_view );
}
