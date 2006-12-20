#ifndef rgrl_matcher_k_nearest_adv_h_
#define rgrl_matcher_k_nearest_adv_h_
//:
// \file
// \author Gehua Yang
// \date   Dec 2006

#include <rgrl/rgrl_matcher_k_nearest.h>
#include <rgrl/rgrl_transformation_sptr.h>

//: For each "from" feature, match the k nearest "to" features.
//
// This will map the "from" feature via the current transform and
// select the k nearest "to" features to the mapped "from" feature.
//
// The some of the nearest features can optionally be discarded if it
// is further than some threshold distance.
//
class rgrl_matcher_k_nearest_adv
  : public rgrl_matcher_k_nearest
{
 public:
  //: Initialize the matcher to select \a k correspondences per "from" feature.
  //
  rgrl_matcher_k_nearest_adv( unsigned int k );

  //: Select at most \a k correspondences within the threshold.
  //
  // The matcher will select \a k correspondences from the list of
  // correspondences with Euclidean distance within the threshold
  // \a dist_thres.
  //
  rgrl_matcher_k_nearest_adv( unsigned int k, double dist_thres );

  rgrl_match_set_sptr
  compute_matches( rgrl_feature_set const&       from_features,
                   rgrl_feature_set const&       to_features,
                   rgrl_view const&              current_view,
                   rgrl_transformation const&    current_xform,
                   rgrl_scale const&             /* current_scale */,
                   rgrl_match_set_sptr const&    old_matches = 0 );


  // Defines type-related functions
  rgrl_type_macro( rgrl_matcher_k_nearest_adv, rgrl_matcher_k_nearest);

 protected:

  //: validate the mapped feature
  inline
  bool validate( rgrl_feature_sptr const& mapped, rgrl_mask_sptr const& roi_sptr ) const;

 protected:
  double min_mapped_scale_;
  double sqr_thres_for_reuse_match_;
  rgrl_transformation_sptr prev_xform_;
};

#endif // rgrl_matcher_k_nearest_adv_h_
