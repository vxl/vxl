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

  // Step3: First check if the regions in the view have converged. If yes,
  //        check the convergence of the transform estimate by comparing to
  //        the previous status. The oscillation count is incremented if the
  //        error_diff changes the sign.
  //
  bool converged;
  bool stagnated = false;
  unsigned int oscillation_count = 0;
  double error_diff = 0.0;
  if (new_error == 0.0 )
    converged = true;
  else if ( prev_status &&  current_view.regions_converged_to(prev_view) ) {
    double old_error = prev_status->error();
    error_diff = new_error-old_error;
    double diff = (new_error-old_error) / new_error ;
    converged = vcl_abs( diff ) < 1e-3;

    if ( !converged ) {
      // look for oscillation
      if ( error_diff * prev_status->error_diff() < 0.0 ) {
        oscillation_count = prev_status->oscillation_count() + 1;
        DebugMacro_abv(1, "Oscillation. Count="<<oscillation_count<<'\n' );

      } else {
        if ( prev_status->oscillation_count() > 0 )
          oscillation_count = prev_status->oscillation_count() - 1;
      }
      if ( oscillation_count > 10 ) {
        stagnated = true;
      }
    }
  } else {
    converged = false;
  }

  return new rgrl_converge_status( converged, stagnated, good, new_error, oscillation_count, error_diff );
}
