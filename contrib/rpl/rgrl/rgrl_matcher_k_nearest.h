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
                   rgrl_scale const&             /* current_scale */ );


  // Defines type-related functions
  rgrl_type_macro( rgrl_matcher_k_nearest, rgrl_matcher);

 protected:

  // nodes to be sorted based on distance
  struct internal_dist_node{
    double geo_err_;
    nodes_vec_iterator itr_;
    
    internal_dist_node(): geo_err_(-1.0){  }
    
    bool operator<( internal_dist_node const& rhs ) const;
  };


  //: This is internal to invert matches function.
  //  It is to restrict the number of nearest neighbors
  virtual
  void
  add_one_flipped_match( rgrl_match_set_sptr&      inv_set,
                         rgrl_view          const& current_view,
                         nodes_vec_iterator const& begin_iter,
                         nodes_vec_iterator const& end_iter ); 
 
 protected:
  unsigned int k_;
  double thres_;
};

#endif // rgrl_matcher_k_nearest_h_
