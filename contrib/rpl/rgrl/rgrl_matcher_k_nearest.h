#ifndef rgrl_matcher_k_nearest_h_
#define rgrl_matcher_k_nearest_h_
//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_matcher.h"

//: For each "from" feature, match the k nearest "to" features.
//
// This will map the "from" feature via the current transform and
// select the k nearest "to" features to the mapped "from" feature.
//
// The some of the nearest features can optionally be discarded if it
// is further than some threshold distance.
//
class rgrl_matcher_k_nearest
  : public rgrl_matcher
{
 public:
  //: Initialize the matcher to select \a k correspondences per "from" feature.
  //
  rgrl_matcher_k_nearest( unsigned int k );

  //: Select at most \a k correspondences within the threshold.
  //
  // The matcher will select \a k correspondences from the list of
  // correspondences with Euclidean distance within the threshold
  // \a dist_thres.
  //
  rgrl_matcher_k_nearest( unsigned int k, double dist_thres );

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
  rgrl_type_macro( rgrl_matcher_k_nearest, rgrl_matcher);

 protected:
  unsigned int k_;
  double thres_;
};

#endif // rgrl_matcher_k_nearest_h_
