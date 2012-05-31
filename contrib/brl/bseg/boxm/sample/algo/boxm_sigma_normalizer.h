// this is brl/bseg/boxm/sample/algo/boxm_sigma_normalizer.h
#ifndef boxm_sigma_normalizer_h_
#define boxm_sigma_normalizer_h_
//:
// \file
// \brief A class for adjusting sample standard deviation values such that the probability of underestimation of the true std. dev. is fixed.
//
// \author Daniel Crispell
// \date   Feb. 18, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_vector.h>

class boxm_sigma_normalizer
{
 public:
  //: create a sigma_normalizer object.
  boxm_sigma_normalizer(float under_estimation_probability);

  //: compute the normalization factor for a sample distribution computing with N observations
  // \a sigma_norm = sample_sigma * normalization_factor(N);
  float normalization_factor(float number_of_observations) const;

  //: compute the normalization factor for a sample distribution computed with N observations
  // \a sigma_norm = sample_sigma * normalization_factor(N);
  float normalization_factor_int(unsigned int number_of_observations) const;

 private:
  // do not allow default constructor - under-estimation param is necessary
  boxm_sigma_normalizer(){}

  static const unsigned int N_PRECOMPUTED_ = 40;
  vcl_vector<float> unbias_const_;
};

#endif
