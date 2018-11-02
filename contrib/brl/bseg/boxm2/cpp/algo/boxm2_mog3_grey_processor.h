// This is brl/bseg/boxm2/cpp/algo/boxm2_mog3_grey_processor.h
#ifndef boxm2_mog3_grey_processor_h_
#define boxm2_mog3_grey_processor_h_
//:
// \file
// \brief A class for a grey-scale-mixture-of-gaussian processor
//
// \author Vishal Jain
// \date   Dec 27, 2010
// \verbatim
//  Modifications
//   moved to bsta/algo kept this facade to minimize code rework
// \endverbatim

#include <iostream>
#include <vector>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bsta/algo/bsta_sigma_normalizer.h>
#include <bsta/algo/bsta_mog3_grey.h>
bool sort_components (vnl_vector_fixed<float,3> i,vnl_vector_fixed<float,3> j) ;


class  boxm2_mog3_grey_processor
{
 public:
  static float expected_color( vnl_vector_fixed<unsigned char, 8> mog3){
    return bsta_mog3_grey::expected_color(mog3);
  }
  static float prob_density( const vnl_vector_fixed<unsigned char, 8> & mog3, float x){
    return bsta_mog3_grey::prob_density(mog3, x);
  }
  static float gauss_prob_density(float x, float mu, float sigma){
    return bsta_mog3_grey::gauss_prob_density(x, mu, sigma);
  }
  static void  update_gauss_mixture_3(vnl_vector_fixed<unsigned char, 8> & mog3,
                                        vnl_vector_fixed<float, 4> & nobs,
                                      float x, float w, float init_sigma,float min_sigma){
    bsta_mog3_grey::update_gauss_mixture_3(mog3, nobs, x, w, init_sigma, min_sigma);
  }
  static void update_app_model(vnl_vector_fixed<unsigned char, 8> & mog3,
                               vnl_vector_fixed<float, 4> & nobs,
                               float x, float w, float init_sigma,float min_sigma){
    bsta_mog3_grey::update_gauss_mixture_3(mog3,nobs,x,w,init_sigma,min_sigma);
  }
  static void  sort_mix_3(float & mu0, float& sigma0, float& w0, float& Nobs0,
                          float & mu1, float& sigma1, float& w1, float& Nobs1,
                          float & mu2, float& sigma2, float& w2, float& Nobs2){
    bsta_mog3_grey::sort_mix_3(mu0, sigma0, w0, Nobs0, mu1, sigma1, w1, Nobs1,
                               mu2, sigma2, w2, Nobs2);
  }
  static void  update_gauss(float & x, float & rho, float & mu, float &  sigma,float min_sigma){
    bsta_mog3_grey::update_gauss(x, rho, mu, sigma, min_sigma);
  }
  static void  insert_gauss_3(float x, float init_weight, float init_sigma, int & match,
                              float& mu0, float& sigma0, float& w0, float& Nobs0,
                              float& mu1, float& sigma1, float& w1, float& Nobs1,
                              float& mu2, float& sigma2, float& w2, float& Nobs2){
    bsta_mog3_grey::insert_gauss_3(x, init_weight, init_sigma, match,
                                   mu0,  sigma0,  w0,  Nobs0,
                                   mu1,  sigma1,  w1,  Nobs1,
                                   mu2,  sigma2,  w2,  Nobs2);
  }

  static float clamp(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }

  static bool merge_gauss(float mu1,float var1, float w1,
                          float mu2,float var2, float w2,
                          vnl_vector_fixed<float, 3> & new_component){
    return bsta_mog3_grey::merge_gauss(mu1, var1, w1, mu2, var2, w2, new_component);
  }

  static void  merge_mixtures(vnl_vector_fixed<unsigned char, 8> & mog3_1, float w1,
                              vnl_vector_fixed<unsigned char, 8> & mog3_2, float w2,
                              vnl_vector_fixed<unsigned char, 8> & mog3_3){
    bsta_mog3_grey::merge_mixtures(mog3_1, w1, mog3_2, w2, mog3_3);
  }

  static void compute_app_model(vnl_vector_fixed<unsigned char, 8> & apm,
                                std::vector<float> const& obs,
                                std::vector<float> const& obs_weights,
                                bsta_sigma_normalizer_sptr n_table,
                                float min_sigma = 0.01f){
    bsta_mog3_grey::compute_app_model(apm, obs, obs_weights, n_table, min_sigma);
  }
  static void compute_app_model(vnl_vector_fixed<unsigned char, 8> & apm,
                                std::vector<float> const& obs,
                                std::vector<float> const& pre,
                                std::vector<float> const& vis,
                                bsta_sigma_normalizer_sptr n_table,
                                float min_sigma=0.01f){
    bsta_mog3_grey::compute_app_model(apm, obs, pre, vis, n_table, min_sigma);
  }
};

#endif // boxm2_mog3_grey_processor_h_
