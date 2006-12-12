#ifndef rgrl_feature_set_location_masked_h_
#define rgrl_feature_set_location_masked_h_
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

#include <rgrl/rgrl_feature_set.h>
#include <rgrl/rgrl_feature_sptr.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_mask_sptr.h>

//: Represents a set of point features, valid in the masked region
//
class rgrl_feature_set_location_masked
  : public rgrl_feature_set
{
 public:

  rgrl_feature_set_location_masked( rgrl_feature_set_sptr const& fea_set,
                                    rgrl_mask_sptr mask )
  : fea_set_sptr_( fea_set ), mask_( mask ),
    rgrl_feature_set( fea_set->all_features(), fea_set->label() )
  {}

  ~rgrl_feature_set_location_masked() {}
  
  //:  Return all the features
  //
  feature_vector const&
  all_features( ) const
  { return fea_set_sptr_->all_features(); }
  
  
  //:  Return the bounding box encloses the feature set
  rgrl_mask_box
  bounding_box() const
  { return fea_set_sptr_->bounding_box(); }
  
  //:  Return the type of feature
  //
  virtual
  const vcl_type_info&
  type() const
  { return fea_set_sptr_->type(); }
  
  feature_vector
  features_in_region( rgrl_mask_box const& roi ) const
  {
    feature_vector final_results;
    feature_vector results = fea_set_sptr_->features_in_region( roi );
    // check if features are in the valid region
    typedef feature_vector::iterator fvec_itr;
    for ( fvec_itr fitr = results.begin(); fitr != results.end(); ++fitr )
      if ( mask_->inside( (*fitr)->location() ) )
        final_results.push_back( *fitr );
    return final_results;
  }

  //:  Return the features in a given circle/sphere.
  //
  feature_vector
  features_within_radius( vnl_vector<double> const& center, double radius ) const
  {
    feature_vector final_results;
    feature_vector results = fea_set_sptr_->features_within_radius( center, radius );
    // check if features are in the valid region
    typedef feature_vector::iterator fvec_itr;
    for ( fvec_itr fitr = results.begin(); fitr != results.end(); ++fitr )
      if ( mask_->inside( (*fitr)->location() ) )
        final_results.push_back( *fitr );
    return final_results;
  }

  //: Nearest feature based on Euclidean distance
  //
  rgrl_feature_sptr
  nearest_feature( rgrl_feature_sptr feature ) const
  {
    return mask_->inside(feature->location()) ?
           fea_set_sptr_->nearest_feature( feature ) :
           (rgrl_feature_sptr)0;
  }

  //: Nearest feature based on Euclidean distance
  //
  rgrl_feature_sptr
  nearest_feature( const vnl_vector<double>& loc ) const
  {
    return mask_->inside(loc) ?
           fea_set_sptr_->nearest_feature( loc ) :
           (rgrl_feature_sptr)0;
  }

  //: Return all features within a given Euclidean distance
  //
  feature_vector
  features_within_distance( rgrl_feature_sptr feature, double distance ) const
  {
    return mask_->inside(feature->location()) ?
           fea_set_sptr_->features_within_distance( feature , distance) :
           feature_vector();
  }

  //: Return the k nearest features based on Euclidean distance
  feature_vector
  k_nearest_features( const vnl_vector<double>& loc, unsigned int k ) const
  {
    feature_vector results;
    return mask_->inside(loc) ?
           fea_set_sptr_->k_nearest_features(loc, k) :
           feature_vector();
  }

  //: Return the k nearest features based on Euclidean distance
  feature_vector
  k_nearest_features( rgrl_feature_sptr feature, unsigned int k ) const
  {
    feature_vector results;
    return mask_->inside(feature->location()) ?
           fea_set_sptr_->k_nearest_features(feature, k) :
           feature_vector();
  }

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_set_location_masked, rgrl_feature_set);

 private:
  rgrl_feature_set_sptr  fea_set_sptr_;
  rgrl_mask_sptr         mask_;
};

#endif
