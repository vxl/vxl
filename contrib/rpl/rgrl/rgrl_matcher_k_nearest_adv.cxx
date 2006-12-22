#include "rgrl_matcher_k_nearest_adv.h"
//:
// \file
// \author Gehua Yang
// \date   Dec 2006

#include <rgrl/rgrl_feature.h>
#include <rgrl/rgrl_feature_set.h>
#include <rgrl/rgrl_transformation.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_match_set.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>

//-----------------------------------------------
//: a struct for fast look up of the from feature
struct feature_sptr_iterator_pair {

  typedef rgrl_match_set::const_from_iterator  from_feature_iterator;
  rgrl_feature_sptr                   feature_;
  from_feature_iterator               fea_iterator_;

  feature_sptr_iterator_pair()
  { }

  feature_sptr_iterator_pair( rgrl_feature_sptr const& fea, from_feature_iterator const& it )
  : feature_( fea), fea_iterator_( it )
  { }

  bool operator< ( feature_sptr_iterator_pair const& rhs ) const
  { return this->feature_ < rhs.feature_; }

  bool operator== ( feature_sptr_iterator_pair const& rhs ) const
  { return this->feature_ == rhs.feature_; }
};

rgrl_matcher_k_nearest_adv::
rgrl_matcher_k_nearest_adv( unsigned int k )
  : rgrl_matcher_k_nearest(k),
    min_mapped_scale_( -1 ),
    sqr_thres_for_reuse_match_( -1 )
{
}


rgrl_matcher_k_nearest_adv::
rgrl_matcher_k_nearest_adv( unsigned int k, double dist_thres, double min_mapped_scale, double thres_reuse_match )
  : rgrl_matcher_k_nearest( k, dist_thres ),
    min_mapped_scale_( min_mapped_scale ),
    sqr_thres_for_reuse_match_( thres_reuse_match )
{
  if ( sqr_thres_for_reuse_match_ > 0 )
    sqr_thres_for_reuse_match_ = sqr_thres_for_reuse_match_*sqr_thres_for_reuse_match_;
}


rgrl_match_set_sptr
rgrl_matcher_k_nearest_adv::
compute_matches( rgrl_feature_set const&       from_set,
                 rgrl_feature_set const&       to_set,
                 rgrl_view const&              current_view,
                 rgrl_transformation const&    current_xform,
                 rgrl_scale const&             /* current_scale */,
                 rgrl_match_set_sptr const&    old_matches )
{
  typedef rgrl_view::feature_vector feat_vector;
  typedef feat_vector::const_iterator feat_iter;

  assert( current_view.xform_estimate().as_pointer() == &current_xform );

  // faster to check a boolean variable
  const bool allow_reuse_match = ( sqr_thres_for_reuse_match_ > 0) && (prev_xform_) && (old_matches);

  // create a map for from feature and the match iterator
  //
  typedef rgrl_match_set::const_from_iterator  from_feature_iterator;
  vcl_map< rgrl_feature_sptr, from_feature_iterator > feature_sptr_iterator_map;
  if ( allow_reuse_match )
  {
    for ( from_feature_iterator i=old_matches->from_begin(); i!=old_matches->from_end(); ++i )
      feature_sptr_iterator_map[ i.from_feature() ] = i;
  }

  // create the new match set
  //
  rgrl_match_set_sptr matches_sptr
    = new rgrl_match_set( from_set.type(), to_set.type(), from_set.label(), to_set.label() );

  //  get the features in the current view
  feat_vector from;
  if ( !current_view.features_in_region( from, from_set ) ) {
    DebugMacro( 1, "Cannot get features in current region!!!\n");
    return matches_sptr;
  }

  // reserve size
  feat_vector matching_features, pruned_set;
  matching_features.reserve( 10 );
  pruned_set.reserve( 10 );

  matches_sptr->reserve( from.size() );

  //  generate the matches for each feature of this feature type in the current region
  vnl_vector<double> prev_mapped;
  int reuse_match_count = 0;
  for ( feat_iter fitr = from.begin(); fitr != from.end(); ++fitr )
  {
    rgrl_feature_sptr mapped = (*fitr)->transform( current_xform );
    if ( !validate( mapped, current_view.to_image_roi() ) )
      continue;   // feature is invalid

    matching_features.clear();

    if ( allow_reuse_match )
    {
      prev_xform_->map_location( (*fitr)->location(), prev_mapped );

      // if the mapping difference is smaller than this threshold
      vcl_map< rgrl_feature_sptr, from_feature_iterator >::const_iterator map_itr;
      if ( vnl_vector_ssd( prev_mapped, mapped->location() ) < sqr_thres_for_reuse_match_  &&
           (map_itr=feature_sptr_iterator_map.find( *fitr )) != feature_sptr_iterator_map.end() )
      {
        // re-use the to features
        const from_feature_iterator& prev_from_iter = map_itr->second;
        for ( from_feature_iterator::to_iterator titr=prev_from_iter.begin(); titr!=prev_from_iter.end(); ++titr )
          matching_features.push_back( titr.to_feature() );

        // increament the count
        ++reuse_match_count;
      }
      else
        to_set.k_nearest_features( matching_features, mapped, k_ );
    }
    else
      to_set.k_nearest_features( matching_features, mapped, k_ );

    // prune the matches to satisfy the threshold
    //
    if ( thres_ > 0 ) {
      pruned_set.clear();
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

  DebugMacro( 1, "There are " << reuse_match_count << " reuse of previous matches out of " << from.size() << vcl_endl );

  // store xform
  prev_xform_ = current_view.xform_estimate();

  return matches_sptr;
}

inline
bool
rgrl_matcher_k_nearest_adv::
validate( rgrl_feature_sptr const& mapped, rgrl_mask_sptr const& roi_sptr ) const
{
  // if the mapped point is not in the image
  if ( !roi_sptr->inside( mapped->location() ) )
    return false;

  // Suppose scale=1 is the lowest scale, or the finest resolution
  // Any mapped scale below 1 cannot find any correspondence
  // due to the pixel discretization.
  // In practice, the threshold is a number less than one,
  // as feature of real scale=0.9 is still likely to be detected.

  // if the scale is too small to be detected on the other image
  const double scale = mapped->scale();
  if ( scale!=0 && min_mapped_scale_>0 && scale<min_mapped_scale_ ) {
    return false;
  }

  // by default,
  return true;
}
