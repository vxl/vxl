#ifndef rgrl_initializer_ran_sam_h_
#define rgrl_initializer_ran_sam_h_
//:
// \file
// \brief Generate the initial estimate using random sampling
// \author Charlene Tsai
// \date Sep 2002

#include "rgrl_initializer.h"
#include "rgrl_match_set_sptr.h"
#include "rgrl_scale_sptr.h"
#include "rgrl_estimator_sptr.h"
#include "rgrl_scale_estimator_sptr.h"
#include "rgrl_mask.h"

#include <vcl_vector.h>

class vnl_random;

//: Generate the initial estimate using random sampling.
//  This is the re-implementation of rrel_ran_sam_search.
//  Modifications include using unweighted scale estimator
//  to perform selection of the best transformation.
//
class rgrl_initializer_ran_sam
  : public rgrl_initializer
{
 public:
  //: Constructor using a non-deterministic random-sampling seed.
  rgrl_initializer_ran_sam( );

  //: Constructor using a given random-sampling seed.
  rgrl_initializer_ran_sam( int seed );

  virtual
  ~rgrl_initializer_ran_sam();

  //  Parameters to control the search technique.  The default set
  //  when the constructor is called is to sample as in generate
  //  samples as specified in least-median of squares.

  //: Indicate that all possible minimal subset samples should be tried.
  void set_gen_all_samples();

  //: Set the parameters for random sampling.
  void set_sampling_params( double max_outlier_frac = 0.5,
                            double desired_prob_good = 0.99,
                            unsigned int max_populations_expected = 1,
                            unsigned int min_samples = 0 );

  //: Initialize the data with a view, which contains the regions and the transformation estimator.
  //
  // If \a should_estimate_global_region is true, the \a
  // from_image_roi will be re-estimated/updated based on the
  // transform estimate computed by the random sampling process.
  //
  void set_data(rgrl_match_set_sptr                init_match_set,
                rgrl_scale_estimator_unwgted_sptr  scale_est,
                rgrl_view_sptr                     prior_view,
                bool should_estimate_global_region = true);

  //: Initialize with a set of information without packing everything into a vie
  void set_data(rgrl_match_set_sptr                init_match_set,
                rgrl_scale_estimator_unwgted_sptr  scale_est,
                rgrl_mask_sptr      const&         from_image_roi,
                rgrl_mask_sptr      const&         to_image_roi,
                rgrl_mask_box       const&         initial_from_image_roi,
                rgrl_estimator_sptr                xform_estimator,
                unsigned                           initial_resolution = 0,
                bool should_estimate_global_region = true);

  //: Initialize with a set of information, assuming that registration applies to \a from_image_roi always.
  void set_data(rgrl_match_set_sptr                init_match_set,
                rgrl_scale_estimator_unwgted_sptr  scale_est,
                rgrl_mask_sptr      const&         from_image_roi,
                rgrl_mask_sptr      const&         to_image_roi,
                rgrl_estimator_sptr                xform_estimator,
                unsigned                           initial_resolution = 0);

  //: Initialize with a set of information, assuming that registration applies to \a from_image_roi always;
  //  And \a from_image_roi and \a to_image_roi are the same
  void set_data(rgrl_match_set_sptr                init_match_set,
                rgrl_scale_estimator_unwgted_sptr  scale_est,
                rgrl_mask_sptr      const&         from_image_roi,
                rgrl_estimator_sptr                xform_estimator,
                unsigned                           initial_resolution = 0);


  //: Get next initial estimate when first called, but return false thereafter.
  bool next_initial( rgrl_view_sptr           & view,
                     rgrl_scale_sptr          & prior_scale);

  //:  Get the scale estimate.
  rgrl_scale_sptr scale() const { return scale_; }

  //:  Get the parameter estimate.
  rgrl_transformation_sptr transformation() const { return xform_; }

  //:  Get the number of samples tested in during estimation.
  int samples_tested() const { return samples_to_take_; }

  // Defines type-related functions
  rgrl_type_macro( rgrl_initializer_ran_sam, rgrl_initializer );

 private:
  //: Estimate the best transform.
  bool estimate();

  //
  //: Calculate number of samples --- non-unique matching estimation problems
  void calc_num_samples( unsigned int num_matches );

  //: Determine the next random sample, filling in the "sample" vector.
  void next_sample( unsigned int taken, unsigned int num_points,
                    vcl_vector<int>& sample,
                    unsigned int points_per_sample );

  //: Extract the matches indexed by the point_indices
  rgrl_match_set_sptr get_matches(const vcl_vector<int>&  point_indices, unsigned int total_num_matches);

  //: For debugging
  void trace_sample( const vcl_vector<int>& point_indices ) const;

 protected:
  rgrl_match_set_sptr      match_set_;
  rgrl_estimator_sptr      transform_estiamtor_;
  rgrl_scale_estimator_unwgted_sptr  scale_estimator_;
  rgrl_view_sptr           init_view_;
  bool                     called_before_;    //  has next_initial been called yet?
  bool                     data_set_;
  //unsigned int num_unique_matches_;

 private:
  // Parameters
  //
  double max_outlier_frac_;
  double desired_prob_good_;
  unsigned int max_populations_expected_;
  unsigned int min_samples_;
  bool generate_all_;
  bool should_estimate_global_region_;

  //: Random number generator.
  // Normally, this will point to the "global" generator, but a could
  // point to a local one if the user wants to specify a seed.
  vnl_random* generator_;
  bool own_generator_;

  // The estimate
  //
  rgrl_transformation_sptr xform_;
  rgrl_scale_sptr scale_;

  // Sampling variables
  //
  unsigned int samples_to_take_;
};

#endif
