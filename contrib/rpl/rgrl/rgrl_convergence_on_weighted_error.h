#ifndef rgrl_convergence_on_weighted_error_h_
#define rgrl_convergence_on_weighted_error_h_
//:
// \file
#include "rgrl_convergence_tester.h"

//: Determine convergence based on the weighted average of geometric errors.
//
class rgrl_convergence_on_weighted_error
  : public rgrl_convergence_tester
{
 public:
  //: Create a tester with a tolerance \a tol.
  //
  // The tolerance value determines if the transformation is
  // considered acceptable. It is not used for the testing of
  // convergence.
  //
  rgrl_convergence_on_weighted_error( double tol );

  virtual ~rgrl_convergence_on_weighted_error();
  
  rgrl_converge_status_sptr
  compute_status( rgrl_converge_status_sptr               prev_status,
                  rgrl_view                        const& prev_view,
                  rgrl_view                        const& current_view,
                  rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
                  rgrl_set_of<rgrl_scale_sptr>     const& current_scales,
                  bool                                    penalize_scaling = false ) const;

  //: verify the final alignment
  virtual
  rgrl_converge_status_sptr 
  verify( rgrl_transformation_sptr         const& xform_estimate,
          rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
          rgrl_set_of<rgrl_scale_sptr>     const& current_scales )const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_convergence_on_weighted_error, rgrl_convergence_tester );

 protected:
  double
  compute_alignment_error( rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets ) const;

  double
  compute_alignment_error( rgrl_match_set_sptr const& current_match_set ) const;

 private:
  double tolerance_;
};

#endif // rgrl_convergence_on_weighted_error_h_
