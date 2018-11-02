#ifndef rgrl_feature_set_bins_2d_h_
#define rgrl_feature_set_bins_2d_h_
//:
// \file
// \author Gehua Yang
// \date   Feb 2006
//
// \verbatim
//  Modifications
//   Gehua Yang -- Modified from rgrl_feature_set_location class.
//      The reason is rsdl_bins has problems on finding k-nearest points
// \endverbatim

#include <iostream>
#include <memory>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <rsdl/rsdl_bins_2d.h>

#include "rgrl_feature.h"
#include "rgrl_feature_set.h"
#include "rgrl_mask.h"

//: A set of features grouped only by N-d location.
//
// This class is used to store a collection of discrete features for
// which proximity is determined only by location. There is no
// concepts of "segments"; each feature is considered independent.
//
class rgrl_feature_set_bins_2d
  : public rgrl_feature_set
{
 public:
  //: Constructor.
  //  \a bin_size is only effective when \a use_bins is set true.
  rgrl_feature_set_bins_2d( feature_vector const& features,
                             double bin_size = 10,
                             rgrl_feature_set_label const& label = rgrl_feature_set_label() );

  ~rgrl_feature_set_bins_2d() override;

  void
  features_in_region( feature_vector& results, rgrl_mask_box const& roi ) const override;

  //:  Return the features in a given circle/sphere.
  //
  void
  features_within_radius( feature_vector& results, vnl_vector<double> const& center, double radius ) const override;

  //: Nearest feature based on Euclidean distance
  rgrl_feature_sptr
  nearest_feature( const vnl_vector<double>& loc ) const override;

  //: Nearest feature based on Euclidean distance
  rgrl_feature_sptr
  nearest_feature( rgrl_feature_sptr const& feature ) const override;

  //: Return all features within a given Euclidean distance
  void
  features_within_radius( feature_vector& results, rgrl_feature_sptr const& feature, double distance ) const override;

  //:  Return the k nearest features based on Euclidean distance.
  void
  k_nearest_features( feature_vector& results, const vnl_vector<double>& feature_loc, unsigned int k ) const override;

  //:  Return the k nearest features based on Euclidean distance.
  void
  k_nearest_features( feature_vector& results, rgrl_feature_sptr const& feature, unsigned int k ) const override;

  //:  Return the bounding box encloses the feature set
  rgrl_mask_box
  bounding_box() const override;

  //:  Return the type of feature
  const std::type_info&
  type() const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_set_bins_2d, rgrl_feature_set );

 private:
  typedef rsdl_bins_2d<double,rgrl_feature_sptr> bin2d_type;

  const std::type_info* feature_type_;

  rgrl_mask_box bounding_box_;

  // Using bins as the data structure
  std::unique_ptr< bin2d_type > bins_2d_;
  // bool use_bins_;

};


#endif // rgrl_feature_set_bins_2d_h_
