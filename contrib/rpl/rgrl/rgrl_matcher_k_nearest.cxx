#include <vector>
#include <iostream>
#include <algorithm>
#include "rgrl_matcher_k_nearest.h"
//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include <rgrl/rgrl_feature.h>
#include <rgrl/rgrl_feature_set.h>
#include <rgrl/rgrl_transformation.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_match_set.h>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


bool
rgrl_matcher_k_nearest::internal_dist_node::
operator<( internal_dist_node const& rhs ) const
{
  return this->geo_err_ < rhs.geo_err_;
}

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
  if ( thres_ > 0.0 )  thres_ = thres_*thres_;
}


rgrl_match_set_sptr
rgrl_matcher_k_nearest::
compute_matches( rgrl_feature_set const&       from_set,
                 rgrl_feature_set const&       to_set,
                 rgrl_view const&              current_view,
                 rgrl_transformation const&    current_xform,
                 rgrl_scale const&             /* current_scale */,
                 rgrl_match_set_sptr const&    /*old_matches*/ )
{
  typedef rgrl_view::feature_vector feat_vector;
  typedef feat_vector::const_iterator feat_iter;

  rgrl_match_set_sptr matches_sptr
    = new rgrl_match_set( from_set.type(), to_set.type(), from_set.label(), to_set.label() );

  //  get the features in the current view
  feat_vector from;
  if( !current_view.features_in_region( from, from_set ) ) {
    DebugMacro( 1, "Cannot get features in current region!!!" << std::endl );
    return matches_sptr;
  }

  // reserve size
  feat_vector matching_features, pruned_set;
  matching_features.reserve( 10 );
  pruned_set.reserve( 10 );

  matches_sptr->reserve( from.size() );

  //  generate the matches for each feature of this feature type in the current region
  for ( feat_iter fitr = from.begin(); fitr != from.end(); ++fitr )
  {
    rgrl_feature_sptr mapped = (*fitr)->transform( current_xform );
    if ( !validate( mapped, current_view.to_image_roi() ) )
      continue;   // feature is invalid

    matching_features.clear();
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

  return matches_sptr;
}


//  It is to restrict the number of nearest neighbors during the inversion.
void
rgrl_matcher_k_nearest::
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

  if ( !validate( mapped, current_view.from_image_roi() ) )
    return;

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

  // Kth element based on distance
  if ( size > k_ )
    std::nth_element( dist_nodes.begin(), dist_nodes.begin()+k_, dist_nodes.end() );

  // setup structure
  std::vector< rgrl_feature_sptr > matching_tos;
  std::vector< double >            sig_wgts;
  matching_tos.reserve( k_ );
  sig_wgts.reserve( k_ );

  for ( unsigned i=0; i<k_ && i<size; ++i )
  {
    if ( thres_ > 0 && dist_nodes[i].geo_err_ > thres_ )
      continue;

    nodes_vec_iterator const& current_node = dist_nodes[i].itr_;
    matching_tos.push_back( current_node->from_ );

    // recompute the signature weight
    //
    // double wgt = current_node->sig_wgt_;
    double wgt = current_node->from_->absolute_signature_weight( mapped );
    sig_wgts.push_back( wgt );
    //sig_wgts.push_back( current_node.itr_->sig_wgt_ );
  }

  // add matches
  if ( ! matching_tos.empty() )
    inv_set->add_feature_matches_and_weights( from, mapped, matching_tos, sig_wgts );
}

bool
rgrl_matcher_k_nearest::
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

#if 0
  // if the scale is too small to be detected on the other image
  const double scale = mapped->scale();
  if ( scale!=0 && scale<0.4 ) {
    return false;
  }
#endif // 0

  // by default,
  return true;
}
