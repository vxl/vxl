#ifndef rgrl_matcher_k_nearest_boundary_h_
#define rgrl_matcher_k_nearest_boundary_h_

//:
// \file
// \author Charlene Tsai
// \date   Sep 2003

#include "rgrl_matcher_k_nearest.h"
#include "rgrl_feature_sptr.h"

#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>

//: For each "from" feature, match the boundary points with the k nearest "to" features.
//
// Allows the same operations as for rgrl_matcher_k_nearest. For each
// center-location match, boundary points are extracted and stored as
// 2 matches.  
//
// FIXME: only deal with 2D for now.
//
class rgrl_matcher_k_nearest_boundary
  : public rgrl_matcher_k_nearest
{
public:
  //: Initialize the matcher to select \a k correspondences per "from" feature.
  //
  rgrl_matcher_k_nearest_boundary( unsigned int k );

  //: Select at most \a k correspondences within the threshold.
  //
  // The matcher will select \a k correspondences from the list of
  // correspondences with Euclidian distance within the threshold
  // \a dist_thres.
  //
  rgrl_matcher_k_nearest_boundary( unsigned int k, double dist_thres );

  rgrl_match_set_sptr
  compute_matches( rgrl_feature_set const&       from_features,
                   rgrl_feature_set const&       to_features,
                   rgrl_view const&              current_view,
                   rgrl_transformation const&    current_xform,
		   rgrl_scale const&             /* current_scale */ ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_matcher_k_nearest_boundary, rgrl_matcher_k_nearest);

private:
  typedef vcl_vector<rgrl_feature_sptr >  feature_vector;


  vbl_array_2d<bool>
  match_boundary_pts(feature_vector const&  mapped_bd_pts, 
                     feature_vector const&  to_boundary_pts) const;

  //: Use dynamic programming to find the assignment matrix that gives the minimum sum of squared residuals for the correct matching.
  vbl_array_2d<bool>
  match_boundary_pts_helper(vbl_array_2d<double> const&  dist_error, 
                            vbl_array_2d<bool> const&    valid, 
                            int                          count, 
                            double&                      obj_value) const;
  
};

#endif // rgrl_matcher_k_nearest_boundary_h_
