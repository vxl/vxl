#ifndef rgrl_feature_set_location_masked_txx_
#define rgrl_feature_set_location_masked_txx_
//:
// \file
// \author Charlene Tsai
// \date   Oct 2003

#include "rgrl_feature_set_location_masked.h"

template<unsigned N>
rgrl_feature_set_location_masked<N>::
rgrl_feature_set_location_masked( feature_vector const& features,
                                  rgrl_mask_sptr mask,
                                  bool use_bins,
                                  double bin_size):
  rgrl_feature_set_location<N>(features, use_bins, bin_size),
  mask_( mask )
{
}

template<unsigned N>
rgrl_feature_set_location_masked<N>::
~rgrl_feature_set_location_masked()
{
}

template<unsigned N>
typename rgrl_feature_set_location_masked<N>::feature_vector
rgrl_feature_set_location_masked<N>::
features_in_region( rgrl_mask_box const& roi ) const
{
  feature_vector final_results, results;

  results = rgrl_feature_set_location<N>::features_in_region( roi );

  // check if features are in the valid region
  typedef typename feature_vector::iterator fvec_itr;
  for ( fvec_itr fitr = results.begin(); fitr != results.end(); ++fitr ) {
    if ( mask_->inside( (*fitr)->location() ) ) {
      final_results.push_back( *fitr );
    }
  }

  return final_results;
}


template<unsigned N>
rgrl_feature_sptr
rgrl_feature_set_location_masked<N>::
nearest_feature( rgrl_feature_sptr feature ) const
{
  if ( !mask_->inside(feature->location()) )
    return 0;

  rgrl_feature_sptr result = rgrl_feature_set_location<N>::nearest_feature( feature );
  return result;
}


template<unsigned N>
typename rgrl_feature_set_location_masked<N>::feature_vector
rgrl_feature_set_location_masked<N>::
features_within_distance( rgrl_feature_sptr feature, double distance ) const
{
  feature_vector results;

  if ( !mask_->inside(feature->location()) )
    return results;

  results = rgrl_feature_set_location<N>::features_within_distance( feature , distance);
  return results;
}


//:  Return the k nearest features based on Euclidean distance.
template<unsigned N>
typename rgrl_feature_set_location_masked<N>::feature_vector
rgrl_feature_set_location_masked<N>::
k_nearest_features( rgrl_feature_sptr feature, unsigned int k ) const
{
  feature_vector results;

  if ( !mask_->inside(feature->location()) )
    return results;

  results = rgrl_feature_set_location<N>::k_nearest_features(feature, k);
  return results;
}

#endif // rgrl_feature_set_location_masked_txx_
