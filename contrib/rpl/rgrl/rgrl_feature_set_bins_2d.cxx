#ifndef rgrl_feature_set_bins_2d_cxx_
#define rgrl_feature_set_bins_2d_cxx_
//:
// \file
// \author Amitha Perera
// \date   Feb 2003
//
// \verbatim
//  modifications:
//  April 2004 Charlene: allow the use of kd_tree and user-defined bin_size.
// \endverbatim

#include <rgrl/rgrl_feature_set_bins_2d.h>

#include <vcl_cassert.h>

#include <rsdl/rsdl_bins.h>
#include <rsdl/rsdl_bins.txx> // to avoid explicit instantiation
#include <rsdl/rsdl_kd_tree.h>

#include <rgrl/rgrl_mask.h>


rgrl_feature_set_bins_2d::
rgrl_feature_set_bins_2d( feature_vector const& features,
                           double bin_size,
                           rgrl_feature_set_label const& label)
  :
  rgrl_feature_set( features, label ),
  bounding_box_( 2 )
{
 // Determine the extents of the data. (And the type.)
  //
  typedef vnl_vector_fixed<double, 2> point_type;
  point_type min;
  point_type max;
 
  // if no features
  if( features.empty() ) {
    min.fill( 0 );
    max.fill( bin_size );

  } else {
    feature_vector::const_iterator itr = features.begin();
    //feature_type_ = (*itr)->type_id();
    feature_type_ = &typeid(*(*itr));
    min = (*itr)->location();
    max = min;
    for ( ; itr != features.end(); ++itr ) {
      vnl_vector<double> const& loc = (*itr)->location();
      assert( loc.size() == 2 );
      for ( unsigned i=0; i < 2; ++i ) {
        if ( loc[i] < min[i] )    min[i] = loc[i];
        if ( loc[i] > max[i] )    max[i] = loc[i];
      }
    }
  }
  bounding_box_.set_x0( min.as_ref() );
  bounding_box_.set_x1( max.as_ref() );

  // Now store the feature points in the chosen data structure
  //
  // Create the bins
  point_type bin_sizes;
  bin_sizes.fill( bin_size );
  bins_2d_.reset( new bin2d_type( min, max, bin_sizes ) );

  // Add the data
  for ( feature_vector::const_iterator itr = features.begin(); itr != features.end(); ++itr ) {
    bins_2d_->add_point( (*itr)->location(), *itr );
  }
}


rgrl_feature_set_bins_2d::
~rgrl_feature_set_bins_2d()
{
}


void
rgrl_feature_set_bins_2d::
features_in_region( feature_vector& results, rgrl_mask_box const& roi ) const
{
  assert( roi.x0().size() == 2 );

  bins_2d_->points_in_bounding_box( roi.x0(), roi.x1(), results );
}

void
rgrl_feature_set_bins_2d::
features_within_radius( feature_vector& results, vnl_vector<double> const& center, double radius ) const
{
  bins_2d_->points_within_radius( center, radius, results );
}

rgrl_feature_sptr
rgrl_feature_set_bins_2d::
nearest_feature( rgrl_feature_sptr const& feature ) const
{
  feature_vector results;
  bins_2d_->n_nearest( feature->location(), 1, results );
  assert( results.size() == 1 );
  return results[0];
}


rgrl_feature_sptr
rgrl_feature_set_bins_2d::
nearest_feature( const vnl_vector<double>& loc ) const
{
  feature_vector results;
  bins_2d_->n_nearest( loc, 1, results );
  assert( results.size() == 1 );
  return results[0];
}


void
rgrl_feature_set_bins_2d::
features_within_radius( feature_vector& results, rgrl_feature_sptr const& feature, double distance ) const
{
  bins_2d_->points_within_radius( feature->location(), distance, results );
}

//:  Return the k nearest features based on Euclidean distance.
void
rgrl_feature_set_bins_2d::
k_nearest_features( feature_vector& results, rgrl_feature_sptr const& feature, unsigned int k ) const
{
  bins_2d_->n_nearest( feature->location(), k, results );
}

//:  Return the k nearest features based on Euclidean distance.
void
rgrl_feature_set_bins_2d::
k_nearest_features( feature_vector& results, const vnl_vector<double> & loc, unsigned int k ) const
{
  bins_2d_->n_nearest( loc, k, results );
}

rgrl_mask_box
rgrl_feature_set_bins_2d::
bounding_box() const
{
  return bounding_box_;
}

const vcl_type_info&
rgrl_feature_set_bins_2d::
type() const
{
  return *feature_type_;
}

#endif // rgrl_feature_set_bins_2d_cxx_
