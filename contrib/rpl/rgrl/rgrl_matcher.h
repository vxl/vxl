#ifndef rgrl_matcher_h_
#define rgrl_matcher_h_

//:
// \file
// \brief  Abstract base class to compute matches for a particular feature type. Multiple matches are allowed for each feature.
// \author Chuck Stewart
// \date   12 Nov 2002

#include "rgrl_match_set_sptr.h"
#include "rgrl_mask.h"
#include "rgrl_object.h"

class rgrl_feature_set;
class rgrl_view;
class rgrl_transformation;
class rgrl_scale;

//: Interface for the routines that compute matches.
//
class rgrl_matcher
  : public rgrl_object
{
public:
  virtual ~rgrl_matcher();

  //: Build the matches with a view (old interface)
  //
  virtual
  rgrl_match_set_sptr 
  compute_matches( rgrl_feature_set const&    from_features,
                   rgrl_feature_set const&    to_features,
                   rgrl_view const&           current_view,
                   rgrl_transformation const& current_xform,
                   rgrl_scale const&          current_scale ) const = 0;

  //: Build the matches with a view (new interface)
  //
  //  Current xform is embedded in the view
  virtual
  rgrl_match_set_sptr 
  compute_matches( rgrl_feature_set const&    from_features,
                   rgrl_feature_set const&    to_features,
                   rgrl_view const&           current_view,
                   rgrl_scale const&          current_scale ) const;

  //: Build the matches without a view
  //
  virtual
  rgrl_match_set_sptr 
  compute_matches( rgrl_feature_set const&    from_features,
                   rgrl_feature_set const&    to_features,
                   rgrl_transformation const& current_xform,
                   rgrl_mask_box const&       region,
                   rgrl_scale const&          current_scale ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_matcher, rgrl_object );

};

#endif
