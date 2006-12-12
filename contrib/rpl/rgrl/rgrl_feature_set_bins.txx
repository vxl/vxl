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
void
rgrl_feature_set_bins<N>::
features_in_region( feature_vector& results, rgrl_mask_box const& roi ) const
{
  assert( roi.x0().size() == N );

  bins_->points_in_bounding_box( roi.x0(), roi.x1(), results );
}

template<unsigned N>
void
rgrl_feature_set_bins<N>::
features_within_radius( feature_vector& results, vnl_vector<double> const& center, double radius ) const
{
  bins_->points_within_radius( center, radius, results );
}

template<unsigned N>
rgrl_feature_sptr
rgrl_feature_set_bins<N>::
nearest_feature( rgrl_feature_sptr const& feature ) const
{
  feature_vector results;
  bins_->n_nearest( feature->location(), 1, results );
  assert( results.size() == 1 );
  return results[0];
}


template<unsigned N>
rgrl_feature_sptr
rgrl_feature_set_bins<N>::
nearest_feature( const vnl_vector<double>& loc ) const
{
  feature_vector results;
  bins_->n_nearest( loc, 1, results );
  assert( results.size() == 1 );
  return results[0];
}


template<unsigned N>
void
rgrl_feature_set_bins<N>::
features_within_distance( feature_vector& results, rgrl_feature_sptr const& feature, double distance ) const
{
  bins_->points_within_radius( feature->location(), distance, results );
}

//:  Return the k nearest features based on Euclidean distance.
template<unsigned N>
void
rgrl_feature_set_bins<N>::
k_nearest_features( feature_vector& results, rgrl_feature_sptr const& feature, unsigned int k ) const
{
  bins_->n_nearest( feature->location(), k, results );
}

//:  Return the k nearest features based on Euclidean distance.
template<unsigned N>
void
rgrl_feature_set_bins<N>::
k_nearest_features( feature_vector& results, const vnl_vector<double> & loc, unsigned int k ) const
{
  bins_->n_nearest( loc, k, results );
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
