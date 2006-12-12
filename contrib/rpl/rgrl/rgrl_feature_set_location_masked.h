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
  bounding_box() const;
  
  //:  Return the type of feature
  //
  virtual
  const vcl_type_info&
  type() const
  { return fea_set_sptr_->type(); }
  
  void
  features_in_region( feature_vector& final_results, rgrl_mask_box const& roi ) const;

  //:  Return the features in a given circle/sphere.
  //
  void
  features_within_radius( feature_vector& final_results, vnl_vector<double> const& center, double radius ) const;

  //: Nearest feature based on Euclidean distance
  //
  rgrl_feature_sptr
  nearest_feature( rgrl_feature_sptr const& feature ) const;

  //: Nearest feature based on Euclidean distance
  //
  rgrl_feature_sptr
  nearest_feature( const vnl_vector<double>& loc ) const;

  //: Return all features within a given Euclidean distance
  //
  void
  features_within_distance( feature_vector& results, rgrl_feature_sptr const& feature, double distance ) const;

  //: Return the k nearest features based on Euclidean distance
  void
  k_nearest_features( feature_vector& results, const vnl_vector<double>& loc, unsigned int k ) const;

  //: Return the k nearest features based on Euclidean distance
  void
  k_nearest_features( feature_vector& results, rgrl_feature_sptr const& feature, unsigned int k ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_set_location_masked, rgrl_feature_set);

 private:
  rgrl_feature_set_sptr  fea_set_sptr_;
  rgrl_mask_sptr         mask_;
};

#endif
