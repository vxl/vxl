#ifndef rgrl_feature_based_registration_h_
#define rgrl_feature_based_registration_h_
//:
// \file
// \brief The engine for feature-based registration.
// \author Charlene Tsai
// \date   Feb 2004

#include "rgrl_object.h"
#include "rgrl_data_manager_sptr.h"
#include "rgrl_convergence_tester_sptr.h"
#include "rgrl_estimator_sptr.h"
#include "rgrl_transformation_sptr.h"
#include "rgrl_set_of.h"
#include "rgrl_scale_sptr.h"
#include "rgrl_initializer_sptr.h"
#include "rgrl_converge_status_sptr.h"
#include "rgrl_match_set_sptr.h"
#include "rgrl_weighter_sptr.h"
#include "rgrl_mask.h"

//: Feature-based registration
//
//  The registration engine takes a data storage of single- or
//  multiple-stage/resolution. Each stage may contain multiple feature
//  types. Different transformation estimators may be assigned to
//  different stages. The limitations of feature-based registration
//  are fixed registration region and estimator at each stage.
//
class rgrl_feature_based_registration: public rgrl_object
{
 public:
  //: Initialize with the data storage and the convergence tester
  rgrl_feature_based_registration( rgrl_data_manager_sptr data,
                                   rgrl_convergence_tester_sptr conv_tester );
  //: Initialize with the data storage.
  //
  //  The default convergence tester is set to median error
  rgrl_feature_based_registration( rgrl_data_manager_sptr data );

  ~rgrl_feature_based_registration();

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_based_registration, rgrl_object );

  //: Running from multiple initial estimates, produced by the initializer during registration
  //
  //  Loop through the set of initial estimates.
  void run( rgrl_initializer_sptr initializer );

  //: Running from a given initial estimate.
  //
  //  Based on if data_->is_multi_feature(), call run_single_feature(.)
  //  or run_multi_feature(.)
  //
  void run( rgrl_mask_box              image_region,
            rgrl_estimator_sptr        estimator,
            rgrl_transformation_sptr   initial_xform,
            rgrl_scale_sptr            prior_scale = 0,
            unsigned                   init_resolution = 0);

  //////////////// functions to access internal data  ////////////////////////

  //:  Return the final, best estimate
  rgrl_transformation_sptr final_transformation() const;

  //:  Return the scales of the best transformation estimate
  rgrl_set_of<rgrl_scale_sptr>  const& final_scales() const;
  rgrl_scale_sptr final_scale() const;

  //:  Return the status of the best transformation estimate.
  rgrl_converge_status_sptr final_status() const;

  //:  The multiple match sets used for the best transformation estimate.
  rgrl_set_of<rgrl_match_set_sptr> const& final_match_sets() const;

  //:  The single match set used for the best transformation estimate.
  rgrl_match_set_sptr final_match_set() const;

  //:  Return the number of initial transformations tested
  //
  //   The value might be greater than 1 if using initializer which
  //   provides a number of initial estimates.
  unsigned num_initial_xforms_tested() const;

  //:  Return true if has a best xform_estimate
  bool has_final_transformation() const;

  //: Set the max number of icp iteration per stage
  //
  //  The default is 25 at initialization
  void set_max_icp_iter( unsigned iter );

  //: Set the expected maximum geometric scale
  //
  //  If \a expected_max_geometric_scale_ is never set, the default
  //  has no effect on the registration.
  void set_expected_max_geometric_scale( double scale);

  //: Set the expected minimum geometric scale
  //
  //  If \a expected_min_geometric_scale_ is never set, the default
  //  has no effect on the registration.
  void set_expected_min_geometric_scale( double scale);

  //: Set the number of iterations during which the scale is to be estimated
  //
  //  If \a iterations_for_scale_est_ is never set, the default
  //  has no effect on the registration.
  void set_iterations_for_scale_est( int iter);

  //: penalize transformation that involves scaling of the registraion area
  void penalize_scaling( bool );

  //: Return the current match sets
  rgrl_set_of<rgrl_match_set_sptr>  const& current_match_sets() const;

  //:  Return the current estimate
  rgrl_transformation_sptr current_transformation() const;

  //:  Return the current stage
  unsigned current_stage() const;

  //:  Return the current iterations_at_stage_
  unsigned iterations_at_current_stage() const;

 private:
  //: registration of single feature type at each stage/resolution
  void register_single_feature( rgrl_mask_box            image_region,
                                rgrl_estimator_sptr      initial_xform_estimator,
                                rgrl_transformation_sptr xform_estimate,
                                rgrl_scale_sptr          prior_scale,
                                unsigned                 init_resolution);

  //: registration of multiple feature types at each stage/resolution
  void register_multi_feature( rgrl_mask_box            image_region,
                               rgrl_estimator_sptr      initial_xform_estimator,
                               rgrl_transformation_sptr xform_estimate,
                               rgrl_scale_sptr          prior_scale,
                               unsigned                 init_resolution);


  //: Scale/shrink the registration region and the \a xform_est according to the \a dim_increase_for_next_stage at \a current_resol
  void initialize_for_next_resolution(  rgrl_mask_box            & image_region,
                                        rgrl_transformation_sptr & xform_estimate,
                                        unsigned                 & current_resol ) const;

 private:
  rgrl_data_manager_sptr data_;
  rgrl_convergence_tester_sptr conv_tester_;

  rgrl_converge_status_sptr          best_status_;
  rgrl_transformation_sptr           best_xform_estimate_;
  rgrl_set_of<rgrl_match_set_sptr>   best_matches_;
  rgrl_set_of<rgrl_scale_sptr>       best_scales_;

  unsigned num_xforms_tested_;
  unsigned max_icp_iter_;
  double   expected_max_geometric_scale_; //for early termination
  double   expected_min_geometric_scale_;
  int      iterations_for_scale_est_;
  bool     should_penalize_scaling_;

  // data members to keep track of current status
  unsigned                          current_stage_;
  unsigned                          iterations_at_stage_;
  rgrl_set_of<rgrl_match_set_sptr>  current_match_sets_;
  rgrl_transformation_sptr          current_xform_estimate_;
};

#endif // rgrl_feature_based_registration_h_
