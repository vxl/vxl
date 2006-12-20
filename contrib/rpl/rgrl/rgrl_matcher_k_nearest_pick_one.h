#ifndef rgrl_matcher_k_nearest_pick_one_h_
#define rgrl_matcher_k_nearest_pick_one_h_
//:
// \file
// \author Gehua Yang
// \date   March 2005
// \verbatim
//  Modifications
//   Gehua Yang - Dec 2006 - change the base class to rgrl_matcher_k_nearest_adv. 
//     This is to allow reuse of the old matches when the mapping difference 
//     is smaller than a threshold.
// \endverbatim

#include <rgrl/rgrl_matcher_k_nearest_adv.h>

//: For each "from" feature, match the k nearest "to" features.
//
// This will map the "from" feature via the current transform and
// select the k nearest "to" features to the mapped "from" feature.
//
// The some of the nearest features can optionally be discarded if it
// is further than some threshold distance.
//
class rgrl_matcher_k_nearest_pick_one
  : public rgrl_matcher_k_nearest_adv
{
 public:
  //: Initialize the matcher to select \a k correspondences per "from" feature.
  //
  rgrl_matcher_k_nearest_pick_one( unsigned int k );

  //: Select at most \a k correspondences within the threshold.
  //
  // The matcher will select \a k correspondences from the list of
  // correspondences with Euclidean distance within the threshold
  // \a dist_thres.
  //
  rgrl_matcher_k_nearest_pick_one( unsigned int k, double dist_thres, double min_mapped_scale = -1, double thres_reuse_match = -1 );

  rgrl_match_set_sptr
  compute_matches( rgrl_feature_set const&       from_features,
                   rgrl_feature_set const&       to_features,
                   rgrl_view const&              current_view,
                   rgrl_transformation const&    current_xform,
                   rgrl_scale const&             /* current_scale */,
                   rgrl_match_set_sptr const& old_matches = 0 );


  // Defines type-related functions
  rgrl_type_macro( rgrl_matcher_k_nearest_pick_one, rgrl_matcher);

 protected:

  //: validate the mapped feature
  inline
  bool validate( rgrl_feature_sptr const& mapped, rgrl_mask_sptr const& roi_sptr ) const;

  //: This is internal to invert matches function.
  //  It is to restrict the number of nearest neighbors
  virtual
  void
  add_one_flipped_match( rgrl_match_set_sptr&      inv_set,
                         rgrl_view          const& current_view,
                         nodes_vec_iterator const& begin_iter,
                         nodes_vec_iterator const& end_iter ); 
 
};

#endif // rgrl_matcher_k_nearest_pick_one_h_
