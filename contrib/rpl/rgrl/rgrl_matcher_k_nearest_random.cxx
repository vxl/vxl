//:
// \file
// \author Gehua Yang
// \date   March 2005
#include "rgrl_matcher_k_nearest_random.h"
#include <rgrl/rgrl_feature.h>
#include <rgrl/rgrl_feature_set.h>
#include <rgrl/rgrl_transformation.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_match_set.h>

#include <vcl_vector.h>
#include <vcl_algorithm.h>

rgrl_matcher_k_nearest_random::
rgrl_matcher_k_nearest_random( unsigned int k, unsigned int max_num )
  : rgrl_matcher_k_nearest( k ),
    max_num_( max_num )
{
}


rgrl_matcher_k_nearest_random::
rgrl_matcher_k_nearest_random( unsigned int k, unsigned int max_num, double dist_thres )
  : rgrl_matcher_k_nearest( k, dist_thres ),
    max_num_( max_num )
{
}


rgrl_match_set_sptr
rgrl_matcher_k_nearest_random::
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

  // reserve size
  matches_sptr->reserve( from.size() );

  // set up a vector of same from size to indicate which feature shall be used
  vcl_vector<bool> to_use( from.size(), true );
  
  if( from.size() > max_num_ )
    generate_random_indices( to_use );
  
  //  generate the matches for each feature of this feature type in the current region
  unsigned int k;
  feat_iter fitr;
  for ( k=0, fitr = from.begin(); fitr != from.end(); ++fitr, ++k ) 
    if( to_use[k] ) {

    rgrl_feature_sptr mapped = (*fitr)->transform( current_xform );
    feat_vector matching_features = to_set.k_nearest_features( mapped, k_ );

    // prune the matches to satisfy the threshold
    //
    if ( thres_ > 0 ) {
      feat_vector pruned_set;
      for ( feat_iter i = matching_features.begin(); i != matching_features.end(); ++i ) {
        if ( vnl_vector_ssd( (*i)->location(), mapped->location() ) < thres_ ) {
          pruned_set.push_back( *i );
        }
      }
      if ( !pruned_set.empty() ) {
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
rgrl_matcher_k_nearest_random::
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

void
rgrl_matcher_k_nearest_random::
generate_random_indices( vcl_vector<bool>& to_use ) const 
{
  const unsigned size = to_use.size();

  // set all entries to false
  vcl_fill( to_use.begin(), to_use.end(), false );
    
  unsigned num = 0;
  while( num < max_num_ ) {
    
    unsigned index = random_.lrand32( size );
    // already marked?
    if( to_use[index] )
      continue;
    
    // mark it
    to_use[index] = true;
    ++num;
  }
}
