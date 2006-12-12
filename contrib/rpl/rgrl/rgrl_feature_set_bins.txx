#ifndef rgrl_feature_set_bins_txx_
#define rgrl_feature_set_bins_txx_
//:
// \file
// \author Amitha Perera
// \date   Feb 2003
//
// \verbatim
//  modifications:
//  April 2004 Charlene: allow the use of kd_tree and user-defined bin_size.
// \endverbatim

#include <rgrl/rgrl_feature_set_bins.h>

#include <vcl_cassert.h>

#include <rsdl/rsdl_bins.h>
#include <rsdl/rsdl_bins.txx> // to avoid explicit instantiation

#include <rgrl/rgrl_mask.h>


template<unsigned N>
rgrl_feature_set_bins<N>::
rgrl_feature_set_bins( feature_vector const& features,
                       double bin_size,
                       rgrl_feature_set_label const& label)
  :
  rgrl_feature_set( features, label ),
  bounding_box_( N )
{
  assert( !features.empty() );

  // Determine the extents of the data. (And the type.)
  //
  typedef typename bin_type::point_type point_type;
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
  // Create the bins
  point_type bin_sizes;
  bin_sizes.fill( bin_size );
  bins_.reset( new bin_type( min, max, bin_sizes ) );

  // Add the data
  for ( itr = features.begin(); itr != features.end(); ++itr ) {
    bins_->add_point( (*itr)->location(), *itr );
  }
}


template<unsigned N>
rgrl_feature_set_bins<N>::
~rgrl_feature_set_bins()
{
}


template<unsigned N>
typename rgrl_feature_set_bins<N>::feature_vector
rgrl_feature_set_bins<N>::
features_in_region( rgrl_mask_box const& roi ) const
{
  feature_vector results;

  assert( roi.x0().size() == N );

  bins_->points_in_bounding_box( roi.x0(), roi.x1(), results );

  return results;
}

template<unsigned N>
typename rgrl_feature_set_bins<N>::feature_vector
rgrl_feature_set_bins<N>::
features_within_radius( vnl_vector<double> const& center, double radius ) const
{
  feature_vector results;

  bins_->points_within_radius( center, radius, results );

  return results;
}

template<unsigned N>
rgrl_feature_sptr
rgrl_feature_set_bins<N>::
nearest_feature( rgrl_feature_sptr feature ) const
{
  feature_vector results = this->k_nearest_features( feature->location(), 1 );
  assert( results.size() == 1 );
  return results[0];
}


template<unsigned N>
rgrl_feature_sptr
rgrl_feature_set_bins<N>::
nearest_feature( const vnl_vector<double>& loc ) const
{
  feature_vector results = this->k_nearest_features( loc, 1 );
  assert( results.size() == 1 );
  return results[0];
}


template<unsigned N>
typename rgrl_feature_set_bins<N>::feature_vector
rgrl_feature_set_bins<N>::
features_within_distance( rgrl_feature_sptr feature, double distance ) const
{
  feature_vector results;

  bins_->points_within_radius( feature->location(), distance, results );

  return results;
}

//:  Return the k nearest features based on Euclidean distance.
template<unsigned N>
typename rgrl_feature_set_bins<N>::feature_vector
rgrl_feature_set_bins<N>::
k_nearest_features( rgrl_feature_sptr feature, unsigned int k ) const
{
  return k_nearest_features( feature->location(), k );
}

//:  Return the k nearest features based on Euclidean distance.
template<unsigned N>
typename rgrl_feature_set_bins<N>::feature_vector
rgrl_feature_set_bins<N>::
k_nearest_features( const vnl_vector<double> & loc, unsigned int k ) const
{
  feature_vector results;

  bins_->n_nearest( loc, k, results );

  return results;
}

template<unsigned N>
rgrl_mask_box
rgrl_feature_set_bins<N>::
bounding_box() const
{
  return bounding_box_;
}

template<unsigned N>
const vcl_type_info&
rgrl_feature_set_bins<N>::
type() const
{
  return *feature_type_;
}

#endif // rgrl_feature_set_bins_txx_
