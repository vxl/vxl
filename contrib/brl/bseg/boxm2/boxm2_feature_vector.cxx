#include <vector>
#include <iostream>
#include <vnl/vnl_math.h>
#include "boxm2_feature_vector.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

std::ostream& operator<<(std::ostream &s, boxm2_feature_vector& f)
{
  for (unsigned i = 0; i < boxm2_feature_vector::SIZE; i++) {
    s << f.get_feature(i) << ' ';
  }
  return s;
}

float prob_correspondence(boxm2_feature_vector& f1, float surface_prob1, boxm2_feature_vector& f2, float surface_prob2, double feature_sim_variance)
{
  float d_square = 0.0;
  for (unsigned i = 0; i < boxm2_feature_vector::SIZE; i++) {
    float dif = f1.get_feature(i)-f2.get_feature(i);
    d_square += dif*dif;
  }
  d_square *= surface_prob1*surface_prob2;
  // assuming prob distribution on distance (between feature vectors) is a normal distribution
  auto dif_prob = (float)(vnl_math::one_over_sqrt2pi/std::sqrt(feature_sim_variance)*std::exp(-0.5*d_square/feature_sim_variance));

  // compute the probability of correspondence of these two samples:
  // possibilities for correspondence are: both surface and both non-surface (a prob space with 4 events)
  float prob_cor = surface_prob1*surface_prob2 + (1.0f-surface_prob1)*(1.0f-surface_prob2);
  return prob_cor*dif_prob;
}
