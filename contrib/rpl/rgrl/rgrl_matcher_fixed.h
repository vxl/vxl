#ifndef rgrl_matcher_fixed_h_
#define rgrl_matcher_fixed_h_

//:
// \file
// \author Charlene Tsai
// \date   Sep 2003
#include "rgrl_matcher.h"

//: Returns a set of pre-computed matches for the current view.
//
class rgrl_matcher_fixed
  : public rgrl_matcher
{
public:
  //: Initialize the matcher with a fixed set of matches
  //
  rgrl_matcher_fixed( rgrl_match_set_sptr  init_match_set_ );

  virtual ~rgrl_matcher_fixed();

  rgrl_match_set_sptr
  compute_matches( rgrl_feature_set const&       /* from_features */,
                   rgrl_feature_set const&       /* to_features */,
                   rgrl_view const&              current_view,
                   rgrl_transformation const&    current_xform,
		   rgrl_scale const&             /* current_scale */ ) const;

  rgrl_match_set_sptr 
  compute_matches( rgrl_feature_set const&    /* from_features */,
		   rgrl_feature_set const&    /* to_features */,
		   rgrl_transformation const& current_xform,
                   rgrl_mask_box const&       region, 
		   rgrl_scale const&          /* current_scale */) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_matcher_fixed, rgrl_matcher );

private:
  rgrl_match_set_sptr      match_set_;
};

#endif // rgrl_matcher_fixed_h_
