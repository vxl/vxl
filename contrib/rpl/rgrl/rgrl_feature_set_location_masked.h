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
// \endverbatim

#include "rgrl_feature_set_location.h"
#include "rgrl_mask.h"
#include "rgrl_mask_sptr.h"

//: Represents a set of point features, valid in the masked region
//
template <unsigned N>
class rgrl_feature_set_location_masked
  : public rgrl_feature_set_location<N>
{
 public:
  typedef typename rgrl_feature_set_location<N>::feature_vector feature_vector;

  rgrl_feature_set_location_masked( feature_vector const& features,
                                    rgrl_mask_sptr mask,
                                    bool use_bins = true,
                                    double bin_size = 10 )
  : rgrl_feature_set_location<N>(features, use_bins, bin_size), mask_( mask ) {}

  ~rgrl_feature_set_location_masked() {}

  feature_vector
  features_in_region( rgrl_mask_box const& roi ) const
  {
    feature_vector final_results;
    feature_vector results = rgrl_feature_set_location<N>::features_in_region( roi );
    // check if features are in the valid region
    typedef typename feature_vector::iterator fvec_itr;
    for ( fvec_itr fitr = results.begin(); fitr != results.end(); ++fitr )
      if ( mask_->inside( (*fitr)->location() ) )
        final_results.push_back( *fitr );
    return final_results;
  }

  //: Nearest feature based on signature error
  //
  rgrl_feature_sptr
  nearest_feature( rgrl_feature_sptr feature ) const
  {
    return mask_->inside(feature->location()) ?
           rgrl_feature_set_location<N>::nearest_feature( feature ) :
           (rgrl_feature_sptr)0;
  }

  //: Return all features within a given Euclidean distance
  //
  feature_vector
  features_within_distance( rgrl_feature_sptr feature, double distance ) const
  {
    return mask_->inside(feature->location()) ?
           rgrl_feature_set_location<N>::features_within_distance( feature , distance) :
           feature_vector();
  }

  //: Return the k nearest features based on Euclidean distance, signature error
  feature_vector
  k_nearest_features( rgrl_feature_sptr feature, unsigned int k ) const
  {
    feature_vector results;
    return mask_->inside(feature->location()) ?
           rgrl_feature_set_location<N>::k_nearest_features(feature, k) :
           feature_vector();
  }

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_set_location_masked, rgrl_feature_set_location<N>);

 private:
  rgrl_mask_sptr mask_;
};

#endif
