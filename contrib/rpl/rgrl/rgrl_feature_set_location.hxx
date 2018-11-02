#ifndef rgrl_feature_set_location_txx_
#define rgrl_feature_set_location_txx_
//:
// \file
// \author Amitha Perera
// \date   Feb 2003
//
// \verbatim
//  Modifications:
//   April 2004 Charlene: allow the use of kd_tree and user-defined bin_size.
// \endverbatim

#include "rgrl_feature_set_location.h"

#include <rgrl/rgrl_mask.h>
#include <rsdl/rsdl_kd_tree.h>

#include <vnl/vnl_vector_fixed.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template<unsigned N>
rgrl_feature_set_location<N>::
rgrl_feature_set_location( feature_vector const& features,
                           rgrl_feature_set_label const& label)
  :
  rgrl_feature_set( features, label ),
  bounding_box_( N )
{
  // Determine the extents of the data. (And the type.)
  //
  typedef vnl_vector_fixed<double, N> point_type;
  point_type min;
  point_type max;

  if ( features.empty() )
  {
    min.fill( 0 );
    max.fill( 0 );
  }
  else
  {
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
  }
  bounding_box_.set_x0( min.as_ref() );
  bounding_box_.set_x1( max.as_ref() );

  // Now store the feature points in the chosen data structure
  //
  // Use kd_tree
  std::vector<rsdl_point> search_pts;
  search_pts.reserve( features.size() );
  for (const auto & feature : features) {
    search_pts.emplace_back(feature->location() );
  }
  kd_tree_ = new rsdl_kd_tree( search_pts );
}


template<unsigned N>
rgrl_feature_set_location<N>::
~rgrl_feature_set_location() = default;


template<unsigned N>
void
rgrl_feature_set_location<N>::
features_in_region( feature_vector& results, rgrl_mask_box const& roi ) const
{
  assert( roi.x0().size() == N );

  // Set the bounding box
  rsdl_point min_point( roi.x0() );
  rsdl_point max_point( roi.x1() );
  rsdl_bounding_box box(min_point, max_point);

  // Extract pts in the bounding box
  clear_temp_storage();
  kd_tree_->points_in_bounding_box( box, temp_points_, temp_point_indices_ );

  // transfer the closest_pts to result
  //
  std::size_t num_pts = temp_point_indices_.size();
  for (std::size_t i = 0; i<num_pts; i++ )
    results.push_back( fea_vec_[temp_point_indices_[i]] );
}

template<unsigned N>
void
rgrl_feature_set_location<N>::
features_within_radius( feature_vector& results, vnl_vector<double> const& center, double radius ) const
{
  // Extract pts
  clear_temp_storage();
  kd_tree_->points_in_radius(  center, radius, temp_points_, temp_point_indices_ );

  // transfer the closest_pts to result
  //
  std::size_t num_pts = temp_point_indices_.size();
  results.reserve( num_pts );
  for (std::size_t i = 0; i<num_pts; i++ )
    results.push_back( fea_vec_[temp_point_indices_[i]] );
}

template<unsigned N>
rgrl_feature_sptr
rgrl_feature_set_location<N>::
nearest_feature( rgrl_feature_sptr const& feature ) const
{
  feature_vector results;
  this->k_nearest_features( results, feature->location(), 1 );
  assert( results.size() == 1 );
  return results[0];
}


template<unsigned N>
rgrl_feature_sptr
rgrl_feature_set_location<N>::
nearest_feature( const vnl_vector<double>& loc ) const
{
  feature_vector results;
  this->k_nearest_features( results, loc, 1 );
  assert( results.size() == 1 );
  return results[0];
}


template<unsigned N>
void
rgrl_feature_set_location<N>::
features_within_radius( feature_vector& results, rgrl_feature_sptr const& feature, double distance ) const
{
  rsdl_point query_point(feature->location());
  clear_temp_storage();
  kd_tree_->points_in_radius( query_point, distance, temp_points_, temp_point_indices_ );

  // transfer the closest_pts to result
  //
  std::size_t num_pts = temp_point_indices_.size();
  for (std::size_t i = 0; i<num_pts; i++ )
    results.push_back( fea_vec_[temp_point_indices_[i]] );
}

//:  Return the k nearest features based on Euclidean distance.
template<unsigned N>
void
rgrl_feature_set_location<N>::
k_nearest_features( feature_vector& results, rgrl_feature_sptr const& feature, unsigned int k ) const
{
  rsdl_point query_point(feature->location());
  clear_temp_storage();
  kd_tree_->n_nearest( query_point, k, temp_points_, temp_point_indices_ );

  // transfer the closest_pts to result
  //
  std::size_t num_pts = temp_point_indices_.size();
  for (std::size_t i = 0; i<num_pts; i++ )
    results.push_back( fea_vec_[temp_point_indices_[i]] );
}

//:  Return the k nearest features based on Euclidean distance.
template<unsigned N>
void
rgrl_feature_set_location<N>::
k_nearest_features( feature_vector& results, const vnl_vector<double> & loc, unsigned int k ) const
{
  rsdl_point query_point(loc);
  clear_temp_storage();
  kd_tree_->n_nearest( query_point, k, temp_points_, temp_point_indices_ );

  // transfer the closest_pts to result
  //
  std::size_t num_pts = temp_point_indices_.size();
  for (std::size_t i = 0; i<num_pts; i++ )
    results.push_back( fea_vec_[temp_point_indices_[i]] );
}

template<unsigned N>
rgrl_mask_box
rgrl_feature_set_location<N>::
bounding_box() const
{
  return bounding_box_;
}

template<unsigned N>
const std::type_info&
rgrl_feature_set_location<N>::
type() const
{
  return *feature_type_;
}

#endif // rgrl_feature_set_location_txx_
