#ifndef rgrl_convergence_on_median_error_h_
#define rgrl_convergence_on_median_error_h_

#include "rgrl_convergence_tester.h"

//: Determine convergence based on the median geometric error.
//
class rgrl_convergence_on_median_error
  : public rgrl_convergence_tester
{
public:
  //: Create a tester with a tolerance \a tol. 
  //
  // The tolerance value determines if the transformation is
  // considered acceptable. It is not used for testing of
  // convergence.
  //
  rgrl_convergence_on_median_error( double tol );

  rgrl_converge_status_sptr 
  compute_status( rgrl_converge_status_sptr               prev_status,
		  rgrl_view                        const& prev_view,
		  rgrl_view                        const& current_view,
		  rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
		  rgrl_set_of<rgrl_scale_sptr>     const& current_scales,
                  bool                                    penalize_scaling = false ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_convergence_on_median_error, rgrl_convergence_tester );

private:
  double tolerance_;
};

#endif // rgrl_convergence_on_median_error_h_
