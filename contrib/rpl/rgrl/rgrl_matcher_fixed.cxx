//:
// \file
// \author Charlene Tsai
// \date   Sep 2003

#include "rgrl_matcher_fixed.h"
#include "rgrl_match_set.h"
#include "rgrl_transformation.h"
#include "rgrl_view.h"

rgrl_matcher_fixed::
rgrl_matcher_fixed( rgrl_match_set_sptr  init_match_set_ ):
  match_set_( init_match_set_ )
{
}

rgrl_matcher_fixed::
~rgrl_matcher_fixed()
{
}

rgrl_match_set_sptr
rgrl_matcher_fixed::
compute_matches( rgrl_feature_set const&       /*from_features*/,
                 rgrl_feature_set const&       /*to_features*/,
                 rgrl_view const&              current_view,
                 rgrl_transformation const&    current_xform,
		 rgrl_scale const&             /* current_scale */ ) const
{
  // Iterators to go over the matches
  //
  typedef rgrl_match_set::from_iterator FIter;
  typedef FIter::to_iterator TIter;

  // extract matches with from-features falling into the current_view
  // from the pre-computed match_set
  
  rgrl_match_set_sptr 
    sub_match_set = new rgrl_match_set(match_set_->from_feature_type(), match_set_->to_feature_type());

  for( FIter fi = match_set_->from_begin(); fi != match_set_->from_end(); ++fi ) {
    rgrl_feature_sptr from_feature = fi.from_feature();
    if ( current_view.region().inside(from_feature->location()) ) {
      vcl_vector<rgrl_feature_sptr> matching_to;
      for ( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
        matching_to.push_back( ti.to_feature());
      }
      sub_match_set->add_feature_and_matches( from_feature, 0, matching_to); 
    }
  }
  sub_match_set->remap_from_features( current_xform );

  return sub_match_set;
}

rgrl_match_set_sptr
rgrl_matcher_fixed::
compute_matches( rgrl_feature_set const&       from_features,
                 rgrl_feature_set const&       to_features,
                 rgrl_transformation const&    current_xform,
                 rgrl_mask_box const&          region, 
		 rgrl_scale const&             current_scale  ) const
{
  rgrl_view view( region, region, region, region, 0, 0, 0);
  return this->compute_matches( from_features, 
                                to_features, 
                                current_xform, 
                                region, 
                                current_scale );
}
