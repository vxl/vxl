#ifndef rgrl_feature_set_location_txx_
#define rgrl_feature_set_location_txx_
//:
// \file
// \author Amitha Perera
// \date   Feb 2003
//
// \verbatim
//  modifications:
//  April 2004 Charlene: allow the use of kd_tree and user-defined bin_size.
// \endverbatim

#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_mask.h>
#include <rsdl/rsdl_kd_tree.h>

#include <vnl/vnl_vector_fixed.h>
#include <vcl_cassert.h>

template<unsigned N>
rgrl_feature_set_location<N>::
rgrl_feature_set_location( feature_vector const& features,
                           rgrl_feature_set_label const& label)
  :
  rgrl_feature_set( features, label ),
  bounding_box_( N )
{
  assert( !features.empty() );

  // Determine the extents of the data. (And the type.)
  //
  typedef vnl_vector_fixed<double, N> point_type;
  point_type min;
  point_type max;
  feature_vector::const_iterator itr = features.begin();
  //feature_type_ = (*itr)->type_id();
  feature_type_ = &typeid(*(*itr));
  min = (*itr)->location();
  max = min;
  for ( ; itr != features.end(); ++itr ) {
    vnl_vector<double> const& loc = (*itr)->location();
    assert( loc.size() == N );
    for ( unsigned i=0; i < N; ++i ) {
      if ( loc[i] < min[i] )    min[i] = loc[i];
      if ( loc[i] > max[i] )    max[i] = loc[i];
    }
  }
  bounding_box_.set_x0( min.as_ref() );
  bounding_box_.set_x1( max.as_ref() );

  // Now store the feature points in the chosen data structure
  //
  // Use kd_tree
  vcl_vector<rsdl_point> search_pts;
  search_pts.reserve( features.size() );
  for ( itr = features.begin(); itr != features.end(); ++itr ) {
    search_pts.push_back( rsdl_point((*itr)->location()) );
  }
  kd_tree_ = new rsdl_kd_tree( search_pts );
}


template<unsigned N>
rgrl_feature_set_location<N>::
~rgrl_feature_set_location()
{
}


template<unsigned N>
typename rgrl_feature_set_location<N>::feature_vector
rgrl_feature_set_location<N>::
features_in_region( rgrl_mask_box const& roi ) const
{
  feature_vector results;

  assert( roi.x0().size() == N );

  // Set the bounding box
  rsdl_point min_point( roi.x0() );
  rsdl_point max_point( roi.x1() );
  rsdl_bounding_box box(min_point, max_point);

  // Extract pts in the bounding box
  vcl_vector<rsdl_point> points_in_box;
  vcl_vector<int> point_indices;
  kd_tree_->points_in_bounding_box( box, points_in_box, point_indices );

  // transfer the closest_pts to result
  //
  unsigned int num_pts = point_indices.size();
  for (unsigned int i = 0; i<num_pts; i++ )
    results.push_back( fea_vec_[point_indices[i]] );
  
  return results;
}

template<unsigned N>
typename rgrl_feature_set_location<N>::feature_vector
rgrl_feature_set_location<N>::
features_within_radius( vnl_vector<double> const& center, double radius ) const
{
  feature_vector results;

  // Extract pts
  vcl_vector<rsdl_point> points_in_box;
  vcl_vector<int> point_indices;
  kd_tree_->points_in_radius(  center, radius, points_in_box, point_indices );

  // transfer the closest_pts to result
  //
  unsigned int num_pts = point_indices.size();
  results.reserve( num_pts );
  for (unsigned int i = 0; i<num_pts; i++ )
    results.push_back( fea_vec_[point_indices[i]] );

  return results;
}

template<unsigned N>
rgrl_feature_sptr
rgrl_feature_set_location<N>::
nearest_feature( rgrl_feature_sptr feature ) const
{
  feature_vector results = this->k_nearest_features( feature->location(), 1 );
  assert( results.size() == 1 );
  return results[0];
}


template<unsigned N>
rgrl_feature_sptr
rgrl_feature_set_location<N>::
nearest_feature( const vnl_vector<double>& loc ) const
{
  feature_vector results = this->k_nearest_features( loc, 1 );
  assert( results.size() == 1 );
  return results[0];
}


template<unsigned N>
typename rgrl_feature_set_location<N>::feature_vector
rgrl_feature_set_location<N>::
features_within_distance( rgrl_feature_sptr feature, double distance ) const
{
  feature_vector results;

  rsdl_point query_point(feature->location());
  vcl_vector<rsdl_point> points;
  vcl_vector<int> indices;
  kd_tree_->points_in_radius( query_point, distance, points, indices );

  // transfer the closest_pts to result
  //
  unsigned int num_pts = indices.size();
  for (unsigned int i = 0; i<num_pts; i++ )
    results.push_back( fea_vec_[indices[i]] );

  return results;
}

//:  Return the k nearest features based on Euclidean distance.
template<unsigned N>
typename rgrl_feature_set_location<N>::feature_vector
rgrl_feature_set_location<N>::
k_nearest_features( rgrl_feature_sptr feature, unsigned int k ) const
{
  return k_nearest_features( feature->location(), k );
}

//:  Return the k nearest features based on Euclidean distance.
template<unsigned N>
typename rgrl_feature_set_location<N>::feature_vector
rgrl_feature_set_location<N>::
k_nearest_features( const vnl_vector<double> & loc, unsigned int k ) const
{
  feature_vector results;

  rsdl_point query_point(loc);
  vcl_vector<rsdl_point> closest_points;
  vcl_vector<int> point_indices;
  kd_tree_->n_nearest( query_point, k, closest_points, point_indices );

  // transfer the closest_pts to result
  //
  unsigned int num_pts = point_indices.size();
  for (unsigned int i = 0; i<num_pts; i++ )
    results.push_back( fea_vec_[point_indices[i]] );

  return results;
}

template<unsigned N>
rgrl_mask_box
rgrl_feature_set_location<N>::
bounding_box() const
{
  return bounding_box_;
}

template<unsigned N>
const vcl_type_info&
rgrl_feature_set_location<N>::
type() const
{
  return *feature_type_;
}

#endif // rgrl_feature_set_location_txx_
