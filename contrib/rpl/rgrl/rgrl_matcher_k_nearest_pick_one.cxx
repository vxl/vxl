#include <rgrl/rgrl_matcher_k_nearest_pick_one.h>
//:
// \file
// \author Gehua Yang
// \date   March 2005

#include <rgrl/rgrl_feature.h>
#include <rgrl/rgrl_feature_set.h>
#include <rgrl/rgrl_transformation.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_match_set.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>


rgrl_matcher_k_nearest_pick_one::
rgrl_matcher_k_nearest_pick_one( unsigned int k )
  : rgrl_matcher_k_nearest( k )
{
}


rgrl_matcher_k_nearest_pick_one::
rgrl_matcher_k_nearest_pick_one( unsigned int k, double dist_thres )
  : rgrl_matcher_k_nearest(k, dist_thres)
{
}


rgrl_match_set_sptr
rgrl_matcher_k_nearest_pick_one::
compute_matches( rgrl_feature_set const&       from_set,
                 rgrl_feature_set const&       to_set,
                 rgrl_view const&              current_view,
                 rgrl_transformation const&    current_xform,
                 rgrl_scale const&             /* current_scale */ )
{
  typedef vcl_vector<rgrl_feature_sptr> feat_vector;
  typedef feat_vector::iterator feat_iter;

  rgrl_match_set_sptr matches_sptr = new rgrl_match_set( from_set.type(), to_set.type());

  //  get the features in the current view
  feat_vector from =
    from_set.features_in_region( current_view.region() );

  // reserve size
  matches_sptr->reserve( from.size() );

  //  generate the matches for each feature of this feature type in the current region
  for ( feat_iter fitr = from.begin(); fitr != from.end(); ++fitr ) {

    rgrl_feature_sptr mapped = (*fitr)->transform( current_xform );
    feat_vector matching_features = to_set.k_nearest_features( mapped, k_ );

    // find the one most similar
    double max_weight = -1;
    rgrl_feature_sptr max_feature;
    for ( feat_iter i = matching_features.begin(); i != matching_features.end(); ++i ) {
      
      if( thres_ > 0 && vnl_vector_ssd( (*i)->location(), mapped->location() ) > thres_ )
         continue;
      
      double wgt = (*i)->absolute_signature_weight( mapped );
      
      if( wgt > max_weight ) {
        max_weight = wgt;
        max_feature = *i;
      }
    }
  
    // Add match
    // for consitent behavior, use 
    // add_feature_and_matches() which computes the signature weight
    // We could also add one match with max_weight, which
    // should be identical action. 
    // 
    
    //feat_vector pruned_set;
    //if( max_feature )
    //  pruned_set.push_back( max_feature );
    //  
    //matches_sptr->add_feature_and_matches( *fitr, mapped, pruned_set );

    if( max_feature )
      matches_sptr->add_feature_and_match( *fitr, mapped, max_feature, max_weight );

  }

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
  const unsigned int size = unsigned( end_iter - begin_iter );
  rgrl_transformation_sptr const& inverse_xform = current_view.inverse_xform_estimate();

  // create from feature and map it via inverse transformation
  rgrl_feature_sptr from = begin_iter->to_;
  rgrl_feature_sptr mapped = from->transform( *inverse_xform );

  // for consistent behavior,
  // pick the k nearest neighbor, 
  // and then pick the one with highest similarity
  // 

  internal_dist_node one;
  // compute the distance
  // REMEMBER: the to is from, from is to. Everything is inversed
  //
  vcl_vector< internal_dist_node > dist_nodes;
  dist_nodes.reserve( size );
  for ( nodes_vec_iterator itr = begin_iter; itr!=end_iter; ++itr ) {
    
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
  if( size > k_ )
    vcl_nth_element( dist_nodes.begin(), dist_nodes.begin()+k_, dist_nodes.end() );
  
  // 2. Most similar feature
  //
  double max_weight = -1.0;
  rgrl_feature_sptr max_feature;
  for( unsigned i=0; i<k_ && i<size; ++i ) {

    if( thres_ > 0 && dist_nodes[i].geo_err_ > thres_ )
      continue;

    rgrl_feature_sptr const& one_fea = dist_nodes[i].itr_->from_;
    double wgt = one_fea->absolute_signature_weight( mapped );
    
    if( wgt > max_weight ) {
      max_weight = wgt;
      max_feature = one_fea;
    }
  }
  
  // add matches
  if( max_feature )
    inv_set->add_feature_and_match( from, mapped, max_feature, max_weight );
}
  