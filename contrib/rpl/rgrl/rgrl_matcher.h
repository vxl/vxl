#ifndef rgrl_matcher_h_
#define rgrl_matcher_h_
//:
// \file
// \brief  Abstract base class to compute matches for a particular feature type. Multiple matches are allowed for each feature.
// \author Chuck Stewart
// \date   12 Nov 2002

#include <rgrl/rgrl_match_set_sptr.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_object.h>
#include <rgrl/rgrl_feature_sptr.h>

#include <vcl_stlfwd.h>

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
                   rgrl_scale const&          current_scale ) = 0;

  //: Build the matches with a view (new interface)
  //
  //  Current xform is embedded in the view
  rgrl_match_set_sptr
  compute_matches( rgrl_feature_set const&    from_features,
                   rgrl_feature_set const&    to_features,
                   rgrl_view const&           current_view,
                   rgrl_scale const&          current_scale );

  //: Build the matches without a view
  //
  rgrl_match_set_sptr
  compute_matches( rgrl_feature_set const&    from_features,
                   rgrl_feature_set const&    to_features,
                   rgrl_transformation const& current_xform,
                   rgrl_mask_box const&       region,
                   rgrl_scale const&          current_scale );

  //: invert the matches according to inverse transformation
  virtual
  rgrl_match_set_sptr
  invert_matches( rgrl_match_set const&    current_set,
                  rgrl_view const&         current_view );

  // Defines type-related functions
  rgrl_type_macro( rgrl_matcher, rgrl_object );

 protected:
  
  //: this struct is used in invert_matches
  struct flip_node{
    rgrl_feature_sptr from_, to_;
    double            sig_wgt_;

    // less than operator
    bool operator<( flip_node const& right ); 

  };
  
  typedef vcl_vector<flip_node>::const_iterator nodes_vec_iterator;
  
  virtual 
  void
  add_one_flipped_match( rgrl_match_set_sptr& inv_set, 
                         rgrl_view const& current_view, 
                         nodes_vec_iterator const& begin_iter, 
                         nodes_vec_iterator const& end_iter );
  
  
};

#endif
