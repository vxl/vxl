#ifndef rgrl_matcher_k_nearest_random_h_
#define rgrl_matcher_k_nearest_random_h_
//:
// \file
// \author Gehua Yang
// \date   March 2005

#include <rgrl/rgrl_matcher_k_nearest.h>
#include <vnl/vnl_random.h>
#include <vcl_stlfwd.h>

//: For each "from" feature, match the k nearest "to" features.
//
//  This is the randomized version of k-nearest. The main point
//  is with enough samples, we can sample some of them. Still getting
//  good alignment wo/ performance penalty.
//
class rgrl_matcher_k_nearest_random
  : public rgrl_matcher_k_nearest
{
 public:
  //: Initialize the matcher to select \a k correspondences per "from" feature.
  //
  rgrl_matcher_k_nearest_random( unsigned int k, unsigned int max_num );

  //: Select at most \a k correspondences within the threshold.
  //
  // The matcher will select \a k correspondences from the list of
  // correspondences with Euclidean distance within the threshold
  // \a dist_thres.
  //
  rgrl_matcher_k_nearest_random( unsigned int k, unsigned int max_num, double dist_thres );

  rgrl_match_set_sptr
  compute_matches( rgrl_feature_set const&       from_features,
                   rgrl_feature_set const&       to_features,
                   rgrl_view const&              current_view,
                   rgrl_transformation const&    current_xform,
                   rgrl_scale const&             /* current_scale */ ) const;

  rgrl_match_set_sptr
  compute_matches( rgrl_feature_set const&       from_features,
                   rgrl_feature_set const&       to_features,
                   rgrl_transformation const&    current_xform,
                   rgrl_mask_box const&          region,
                   rgrl_scale const&             /* current_scale */ ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_matcher_k_nearest_random, rgrl_matcher);

 protected:
 
  void generate_random_indices( vcl_vector<bool>& to_use ) const;
  
 protected:
  unsigned int          max_num_;
  mutable vnl_random   random_;
};

#endif // rgrl_matcher_k_nearest_random_h_
