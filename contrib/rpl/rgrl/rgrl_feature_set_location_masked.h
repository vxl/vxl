#ifndef rgrl_feature_set_location_masked_h_
#define rgrl_feature_set_location_masked_h_
//:
// \file
// \brief Derived class to represent point feature set with masked region
// \author Charlene Tsai
// \date   Sep 2003
//

#include "rgrl_feature_set_location.h"
#include "rgrl_mask.h"
#include "rgrl_mask_sptr.h"

//: Represents a set of point features, valid in the masked region
//
template<unsigned N>
class rgrl_feature_set_location_masked
  : public rgrl_feature_set_location<N>
{
 public:
  typedef typename rgrl_feature_set_location<N>::feature_vector feature_vector;

 public:
  rgrl_feature_set_location_masked( feature_vector const& features,
                                    rgrl_mask_sptr mask,
                                    bool use_bins = true,
                                    double bin_size = 10 );

  ~rgrl_feature_set_location_masked();

  feature_vector
  features_in_region( rgrl_mask_box const& roi ) const;

  //: Nearest feature based on signature error
  //
  rgrl_feature_sptr
  nearest_feature( rgrl_feature_sptr feature ) const;

  //: Return all features within a given Euclidean distance
  //
  feature_vector
  features_within_distance( rgrl_feature_sptr feature, double distance ) const;

  //: Return the k nearest features based on signature error
  feature_vector
  k_nearest_features( rgrl_feature_sptr feature, unsigned int k ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_set_location_masked, rgrl_feature_set_location<N>);

 private:
  rgrl_mask_sptr mask_;
};

#endif
