//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_matcher_k_nearest.h"

#include "rgrl_feature.h"
#include "rgrl_feature_set.h"
#include "rgrl_transformation.h"
#include "rgrl_view.h"
#include "rgrl_match_set.h"

#include <vnl/vnl_vector.h>

rgrl_matcher_k_nearest::
rgrl_matcher_k_nearest( unsigned int k )
  : k_( k ),
    thres_( -1.0 )
{
}


rgrl_matcher_k_nearest::
rgrl_matcher_k_nearest( unsigned int k, double dist_thres )
  : k_( k ),
    thres_( dist_thres )
{
}


rgrl_match_set_sptr
rgrl_matcher_k_nearest::
compute_matches( rgrl_feature_set const&       from_set,
                 rgrl_feature_set const&       to_set,
                 rgrl_view const&              current_view,
                 rgrl_transformation const&    current_xform,
		 rgrl_scale const&             /* current_scale */ ) const
{
  typedef vcl_vector<rgrl_feature_sptr> feat_vector;
  typedef feat_vector::iterator feat_iter;

  rgrl_match_set_sptr matches_sptr = new rgrl_match_set( from_set.type(), to_set.type());

  //  get the features in the current view
  feat_vector from =
    from_set.features_in_region( current_view.region() );

  //  generate the matches for each feature of this feature type in the current region
  for ( feat_iter fitr = from.begin(); fitr != from.end(); ++fitr ) {
    rgrl_feature_sptr mapped = (*fitr)->transform( current_xform );
    feat_vector matching_features	= to_set.k_nearest_features( mapped, k_ );

    // prune the matches to satisfy the threshold
    //
    if( thres_ > 0 ) {
      feat_vector pruned_set;
      for( feat_iter i = matching_features.begin(); i != matching_features.end(); ++i ) {
        if( vnl_vector_ssd( (*i)->location(), mapped->location() ) < thres_ ) {
          pruned_set.push_back( *i );
        }
      }
      if( !pruned_set.empty() ) {
        matches_sptr->add_feature_and_matches( *fitr, mapped, 
                                               pruned_set );
      }
    } else {
      matches_sptr->add_feature_and_matches( *fitr, mapped, 
                                             matching_features );
    }
  }

  return matches_sptr;
}

rgrl_match_set_sptr
rgrl_matcher_k_nearest::
compute_matches( rgrl_feature_set const&       from_set,
                 rgrl_feature_set const&       to_set,
                 rgrl_transformation const&    current_xform,
                 rgrl_mask_box const&          region, 
		 rgrl_scale const&             current_scale ) const
{
  rgrl_view view( region, region, region, region, 0, 0, 0);

  return this->compute_matches(from_set, 
                               to_set, 
                               view, 
                               current_xform, 
                               current_scale);
}
