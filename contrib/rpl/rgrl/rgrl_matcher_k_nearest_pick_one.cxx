#include <vector>
#include <iostream>
#include <algorithm>
#include "rgrl_matcher_k_nearest_pick_one.h"
//:
// \file
// \author Gehua Yang
// \date   March 2005

#include <rgrl/rgrl_feature.h>
#include <rgrl/rgrl_feature_set.h>
#include <rgrl/rgrl_transformation.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_match_set.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


rgrl_matcher_k_nearest_pick_one::
rgrl_matcher_k_nearest_pick_one( unsigned int k )
  : rgrl_matcher_k_nearest_adv( k )
{
}


rgrl_matcher_k_nearest_pick_one::
rgrl_matcher_k_nearest_pick_one( unsigned int k, double dist_thres, double min_mapped_scale, double thres_reuse_match )
  : rgrl_matcher_k_nearest_adv(k, dist_thres, min_mapped_scale, thres_reuse_match)
{
}


rgrl_match_set_sptr
rgrl_matcher_k_nearest_pick_one::
compute_matches( rgrl_feature_set const&       from_set,
                 rgrl_feature_set const&       to_set,
                 rgrl_view const&              current_view,
                 rgrl_transformation const&    current_xform,
                 rgrl_scale const&             /* current_scale */,
                 rgrl_match_set_sptr const&    old_matches )
{
  typedef rgrl_view::feature_vector feat_vector;
  typedef feat_vector::const_iterator feat_iter;

  DebugMacro( 2, "Compute matches between features "
                 << from_set.label().name() << "-->"
                 << to_set.label().name() << std::endl; );

  // faster to check a boolean variable
  const bool allow_reuse_match = ( sqr_thres_for_reuse_match_ > 0) && (prev_xform_) && (old_matches);

  // create a map for from feature and the match iterator
  //
  typedef rgrl_match_set::const_from_iterator  from_feature_iterator;
  std::map< rgrl_feature_sptr, from_feature_iterator > feature_sptr_iterator_map;
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
    DebugMacro( 1, "Cannot get features in current region!!!" << std::endl );
    return matches_sptr;
  }

  // reserve size
  feat_vector matching_features;
  matching_features.reserve( 10 );

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
      std::map< rgrl_feature_sptr, from_feature_iterator >::const_iterator map_itr;
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

    if ( debug_flag()>=4 )
    {
      std::cout << " From feature: ";
      (*fitr)->write( std::cout );
      std::cout << std::endl;
      for ( unsigned i=0; i<matching_features.size(); ++i ) {
        std::cout << " ###### " << i << " ###### " << std::endl;
        matching_features[i]->write(std::cout);
      }
    }


    // find the one most similar
    double max_weight = -1;
    double min_eucl_dist = 1e10;
    rgrl_feature_sptr max_feature;
    for ( feat_iter i = matching_features.begin(); i != matching_features.end(); ++i )
    {
      double eucl_dist = vnl_vector_ssd( (*i)->location(), mapped->location() );
      if ( thres_ > 0 && eucl_dist > thres_ )
         continue;

      double wgt = (*i)->absolute_signature_weight( mapped );

      if ( wgt > max_weight )
      {
        // more similar feature is always preferred.
        max_weight = wgt;
        min_eucl_dist = eucl_dist;
        max_feature = *i;
      }
      else if ( std::abs( wgt-max_weight) < 1e-12 && eucl_dist < min_eucl_dist )
      {
        // when the weights are approximately the same,
        // but one point is closer than the other,
        // use the closer point
        max_weight = wgt;
        min_eucl_dist = eucl_dist;
        max_feature = *i;
      }
    }

    // Add match
    // for consistent behavior, use
    // add_feature_and_matches() which computes the signature weight
    // We could also add one match with max_weight, which
    // should be identical action.
    //

    //feat_vector pruned_set;
    //if ( max_feature )
    //  pruned_set.push_back( max_feature );
    //
    //matches_sptr->add_feature_and_matches( *fitr, mapped, pruned_set );

    if ( max_feature )
    {
      matches_sptr->add_feature_and_match( *fitr, mapped, max_feature, max_weight );
      DebugMacro( 4, " ====== Final Choice ======\n" );
      if ( debug_flag() >=4 ) {
        max_feature->write(std::cout);
        std::cout << std::endl;
      }
    }
  }

  DebugMacro( 1, "There are " << reuse_match_count << " reuse of previous matches out of " << from.size() << std::endl );

  // store xform
  prev_xform_ = current_view.xform_estimate();

  return matches_sptr;
}


//  It is to restrict the number of nearest neighbors during the inversion.
void
rgrl_matcher_k_nearest_pick_one::
add_one_flipped_match( rgrl_match_set_sptr&      inv_set,
                       rgrl_view          const& current_view,
                       nodes_vec_iterator const& begin_iter,
                       nodes_vec_iterator const& end_iter )
{
  const auto size = unsigned( end_iter - begin_iter );
  rgrl_transformation_sptr const& inverse_xform = current_view.inverse_xform_estimate();

  // create from feature and map it via inverse transformation
  rgrl_feature_sptr from = begin_iter->to_;
  rgrl_feature_sptr mapped = from->transform( *inverse_xform );

  // check mapped
  if ( !validate( mapped, current_view.from_image_roi() ) )
    return;

  // for consistent behavior,
  // pick the k nearest neighbor,
  // and then pick the one with highest similarity
  //

  // compute the distance
  // REMEMBER: the to is from, from is to. Everything is inversed
  //
  std::vector< internal_dist_node > dist_nodes;
  dist_nodes.reserve( size );
  for ( nodes_vec_iterator itr = begin_iter; itr!=end_iter; ++itr )
  {
    internal_dist_node tmp_node;

    // NOTE: should not use rgrl_feature::geometric_error() function,
    //       because it may compute normal distance, which is not desired
    //
    tmp_node.geo_err_ = vnl_vector_ssd( itr->from_->location(), mapped->location() );
    tmp_node.itr_ = itr;

    // push back
    dist_nodes.push_back( tmp_node );
  }

  // 1. Kth element based on distance
  //
  if ( size > k_ )
    std::nth_element( dist_nodes.begin(), dist_nodes.begin()+k_, dist_nodes.end() );

  // 2. Most similar feature
  //
  double max_weight = -1.0;
  double min_eucl_dist = 1e10;
  rgrl_feature_sptr max_feature;
  for ( unsigned i=0; i<k_ && i<size; ++i )
  {
    double eucl_dist = dist_nodes[i].geo_err_;
    if ( thres_ > 0 && eucl_dist > thres_ )
      continue;

    // one_fea is a feature on the "TO" image. Here the "TO" image
    // is the actual From image.
    rgrl_feature_sptr const& one_fea = dist_nodes[i].itr_->from_; // from is reversed as to
    double wgt = one_fea->absolute_signature_weight( mapped );

    if ( wgt > max_weight )
    {
      // more similar feature is always preferred.
      max_weight = wgt;
      min_eucl_dist = eucl_dist;
      max_feature = one_fea;
    }
    else if ( std::abs( wgt-max_weight) < 1e-12 && eucl_dist < min_eucl_dist )
    {
      // when the weights are approximately the same,
      // but one point is closer than the other,
      // use the closer point
      max_weight = wgt;
      min_eucl_dist = eucl_dist;
      max_feature = one_fea;
    }
  }

  // add matches
  if ( max_feature )
    inv_set->add_feature_and_match( from, mapped, max_feature, max_weight );
}

inline
bool
rgrl_matcher_k_nearest_pick_one::
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
