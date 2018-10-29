#include <vector>
#include <iostream>
#include <algorithm>
#include "rgrl_convergence_on_median_error.h"

#include <vcl_compiler.h>
#include <cassert>

#include "rgrl_match_set.h"
#include "rgrl_set_of.h"
#include "rgrl_converge_status.h"
#include "rgrl_view.h"
#include "rgrl_util.h"

rgrl_convergence_on_median_error::
rgrl_convergence_on_median_error( double tol )
  : tolerance_( tol )
{
}


rgrl_converge_status_sptr
rgrl_convergence_on_median_error::
compute_status( rgrl_converge_status_sptr               prev_status,
                rgrl_view                        const& prev_view,
                rgrl_view                        const& current_view,
                rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
                rgrl_set_of<rgrl_scale_sptr>     const& /*current_scales*/,
                bool                                    penalize_scaling) const
{
  typedef rgrl_match_set::const_from_iterator from_iter;
  typedef from_iter::to_iterator              to_iter;

  // Step1: Compute the errors of all matches
  //
  //rgrl_transformation_sptr current_xform = current_view.xform_estimate();
  std::vector<double> errors;

  for ( unsigned ds=0; ds < current_match_sets.size(); ++ds ) {
    rgrl_match_set const& ms = *current_match_sets[ds];
    for ( from_iter fitr = ms.from_begin(); fitr != ms.from_end(); ++fitr ) {
      //rgrl_feature_sptr mapped = fitr.from_feature()->transform( *current_xform );
      rgrl_feature_sptr mapped = fitr.mapped_from_feature();
      for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
        errors.push_back( titr.to_feature()->geometric_error( *mapped ) );
      }
    }
  }

  // Step2: Take the median of the sorted error vector as the
  //        objective value.  To avoid the low error given by an infeasible
  //        transformation, the error is scaled by the scaling of the spread
  //        of the transformed data points
  //
  assert ( errors.size() > 0 );
  auto middle = errors.begin() + errors.size()/2;
  std::nth_element( errors.begin(), middle, errors.end() );

  double scaling = 1;
  if ( penalize_scaling ) {
    scaling =  rgrl_util_geometric_error_scaling( current_match_sets );
    DebugMacro(1, "geometric_error_scaling = "<<scaling<<'\n');
  }

  double new_error = *middle * scaling;
  bool good = new_error < tolerance_;

  return compute_status_helper( new_error, good, prev_status, prev_view, current_view );
}
