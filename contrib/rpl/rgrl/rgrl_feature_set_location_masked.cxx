#include "rgrl_feature_set_location_masked.h"
//:
// \file
// \brief Derived class to represent point feature set with masked region
// \author Charlene Tsai
// \date   Sep 2003
//
// \verbatim
//  Modifications
//   Peter Vanroose - 14 aug 2004 - moved all impl from .txx to .h to avoid VC60 internal compile error
//   Chuck Stewart - 8 Nov 2005 - added versions of nearest_feature and k_nearest_feature
//      based on point location alone
// \endverbatim

#include <rgrl/rgrl_feature_sptr.h>
#include <rgrl/rgrl_mask.h>

//:  Return the bounding box encloses the feature set
rgrl_mask_box
rgrl_feature_set_location_masked::
bounding_box() const
{ return fea_set_sptr_->bounding_box(); }

void
rgrl_feature_set_location_masked::
features_in_region( feature_vector& final_results, rgrl_mask_box const& roi ) const
{
  feature_vector results;
  fea_set_sptr_->features_in_region( results, roi );
  // check if features are in the valid region
  typedef feature_vector::iterator fvec_itr;
  for ( fvec_itr fitr = results.begin(); fitr != results.end(); ++fitr )
    if ( mask_->inside( (*fitr)->location() ) )
      final_results.push_back( *fitr );
}

//:  Return the features in a given circle/sphere.
//
void
rgrl_feature_set_location_masked::
features_within_radius( feature_vector& final_results, vnl_vector<double> const& center, double radius ) const
{
  feature_vector results;
  fea_set_sptr_->features_within_radius( results, center, radius );
  // check if features are in the valid region
  typedef feature_vector::iterator fvec_itr;
  for ( fvec_itr fitr = results.begin(); fitr != results.end(); ++fitr )
    if ( mask_->inside( (*fitr)->location() ) )
      final_results.push_back( *fitr );
}

//: Nearest feature based on Euclidean distance
//
rgrl_feature_sptr
rgrl_feature_set_location_masked::
nearest_feature( rgrl_feature_sptr const& feature ) const
{
  return mask_->inside(feature->location()) ?
         fea_set_sptr_->nearest_feature( feature ) :
         (rgrl_feature_sptr)0;
}

//: Nearest feature based on Euclidean distance
//
rgrl_feature_sptr
rgrl_feature_set_location_masked::
nearest_feature( const vnl_vector<double>& loc ) const
{
  return mask_->inside(loc) ?
         fea_set_sptr_->nearest_feature( loc ) :
         (rgrl_feature_sptr)0;
}

//: Return all features within a given Euclidean distance
//
void
rgrl_feature_set_location_masked::
features_within_radius( feature_vector& results, rgrl_feature_sptr const& feature, double distance ) const
{
  if ( mask_->inside(feature->location()) )
    fea_set_sptr_->features_within_radius(results, feature , distance);
}

//: Return the k nearest features based on Euclidean distance
void
rgrl_feature_set_location_masked::
k_nearest_features( feature_vector& results, const vnl_vector<double>& loc, unsigned int k ) const
{
  if ( mask_->inside(loc) )
    fea_set_sptr_->k_nearest_features(results, loc, k);
}

//: Return the k nearest features based on Euclidean distance
void
rgrl_feature_set_location_masked::
k_nearest_features( feature_vector& results, rgrl_feature_sptr const& feature, unsigned int k ) const
{
  if ( mask_->inside(feature->location()) )
    fea_set_sptr_->k_nearest_features(results, feature, k);
}
