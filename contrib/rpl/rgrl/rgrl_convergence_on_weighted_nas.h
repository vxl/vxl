#ifndef rgrl_convergence_on_weighted_nas_h_
#define rgrl_convergence_on_weighted_nas_h_
//:
// \file
#include "rgrl_convergence_on_weighted_error.h"

//: Determine convergence based on the weighted average of geometric errors.
//
class rgrl_convergence_on_weighted_nas
  : public rgrl_convergence_on_weighted_error
{
 public:
  //: Create a tester with a tolerance \a tol.
  //
  // The tolerance value determines if the transformation is
  // considered acceptable. It is not used for the testing of
  // convergence.
  //
  rgrl_convergence_on_weighted_nas( double tol, double scaling_ratio_threshold = 0.25 );

  virtual ~rgrl_convergence_on_weighted_nas();
  
  rgrl_converge_status_sptr
  compute_status( rgrl_converge_status_sptr               prev_status,
                  rgrl_view                        const& prev_view,
                  rgrl_view                        const& current_view,
                  rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
                  rgrl_set_of<rgrl_scale_sptr>     const& current_scales,
                  bool                                    penalize_scaling = false ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_convergence_on_weighted_nas, rgrl_convergence_on_weighted_error );

 protected:

  //: initialize status
  virtual rgrl_converge_status_sptr
  init_status( rgrl_view       const& init_view,
               rgrl_scale_sptr const& prior_scale,
               bool                   penalize_scaling ) const;

 protected:
  double scaling_ratio_threshold_;
};

#endif // rgrl_convergence_on_weighted_nas_h_
