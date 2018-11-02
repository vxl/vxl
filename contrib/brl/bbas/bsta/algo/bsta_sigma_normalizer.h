// this is brl/bseg/bbas/algo/bsta_sigma_normalizer.h
#ifndef bsta_sigma_normalizer_h_
#define bsta_sigma_normalizer_h_
//:
// \file
// \brief A class for adjusting sample standard deviation values such that the probability of underestimation of the true std. dev. is fixed.
//
// \author Daniel Crispell
// \date   Feb. 18, 2010
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - moved from boxm/util to bsta/algo
// \endverbatim

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>

class bsta_sigma_normalizer : public vbl_ref_count
{
 public:
  //: create a sigma_normalizer object.
  bsta_sigma_normalizer(float under_estimation_probability, unsigned int N_PRECOMPUTED = 40);

  //: compute the normalization factor for a sample distribution computing with N observations
  // \a sigma_norm = sample_sigma * normalization_factor(N);
  float normalization_factor(float number_of_observations) const;

  //: compute the normalization factor for a sample distribution computed with N observations
  // \a sigma_norm = sample_sigma * normalization_factor(N);
  float normalization_factor_int(unsigned int number_of_observations) const;

 private:
  // do not allow default constructor - under-estimation param is necessary
  bsta_sigma_normalizer()= default;

  unsigned int N_PRECOMPUTED_;
  std::vector<float> unbias_const_;
};

typedef vbl_smart_ptr<bsta_sigma_normalizer> bsta_sigma_normalizer_sptr;

//--- IO read/write for sptrs--------------------------------------------------
//: Binary write bsta_sigma_normalizer to stream
void vsl_b_write(vsl_b_ostream& os, bsta_sigma_normalizer const& scene);
void vsl_b_write(vsl_b_ostream& os, const bsta_sigma_normalizer* &p);
void vsl_b_write(vsl_b_ostream& os, bsta_sigma_normalizer_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, bsta_sigma_normalizer_sptr const& sptr);

//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, bsta_sigma_normalizer &scene);
void vsl_b_read(vsl_b_istream& is, bsta_sigma_normalizer* p);
void vsl_b_read(vsl_b_istream& is, bsta_sigma_normalizer_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, bsta_sigma_normalizer_sptr const& sptr);

#endif
