// This is brl/bbas/algo/bsta_mog3_grey.h
#ifndef bsta_mog3_grey_h_
#define bsta_mog3_grey_h_
//:
// \file
// \brief A class for a grey-scale-mixture-of-gaussian processor
//
// \author Vishal Jain
// \date   Dec 27, 2010
// \verbatim
//  Modifications
//  ported to bsta from boxm2/cpp/algo  JLM
//  April 2016
// \endverbatim

#include <iostream>
#include <vector>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bsta/algo/bsta_sigma_normalizer.h>

bool sort_components (vnl_vector_fixed<float,3> i,vnl_vector_fixed<float,3> j) ;


class  bsta_mog3_grey
{
 public:
     static float expected_color( vnl_vector_fixed<unsigned char, 8> mog3);
     static float prob_density( const vnl_vector_fixed<unsigned char, 8> & mog3, float x);
     static float gauss_prob_density(float x, float mu, float sigma);
     static void  update_gauss_mixture_3(vnl_vector_fixed<unsigned char, 8> & mog3,
                                        vnl_vector_fixed<float, 4> & nobs,
                                        float x, float w, float init_sigma,float min_sigma);
     static void update_app_model(vnl_vector_fixed<unsigned char, 8> & mog3,
                                        vnl_vector_fixed<float, 4> & nobs,
                                        float x, float w, float init_sigma,float min_sigma)
     {
       update_gauss_mixture_3(mog3,nobs,x,w,init_sigma,min_sigma);
     }
     static void  sort_mix_3(float & mu0, float& sigma0, float& w0, float& Nobs0,
                            float & mu1, float& sigma1, float& w1, float& Nobs1,
                            float & mu2, float& sigma2, float& w2, float& Nobs2);
     static void  update_gauss(float & x, float & rho, float & mu, float &  sigma,float min_sigma);
     static void  insert_gauss_3(float x, float init_weight, float init_sigma, int & match,
                                float& mu0, float& sigma0, float& w0, float& Nobs0,
                                float& mu1, float& sigma1, float& w1, float& Nobs1,
                                float& mu2, float& sigma2, float& w2, float& Nobs2);
     static float clamp(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }

     static bool merge_gauss(float mu1,float var1, float w1,
                            float mu2,float var2, float w2,
                            vnl_vector_fixed<float, 3> & new_component);

     static void  merge_mixtures(vnl_vector_fixed<unsigned char, 8> & mog3_1, float w1,
                                 vnl_vector_fixed<unsigned char, 8> & mog3_2, float w2,
                                 vnl_vector_fixed<unsigned char, 8> & mog3_3);

     static void compute_app_model(vnl_vector_fixed<unsigned char, 8> & apm,
                                         std::vector<float> const& obs,
                                         std::vector<float> const& obs_weights,
                                         const bsta_sigma_normalizer_sptr& n_table,
                                         float min_sigma = 0.01f);
     static void compute_app_model(vnl_vector_fixed<unsigned char, 8> & apm,
                                   std::vector<float> const& obs,
                                   std::vector<float> const& pre,
                                   std::vector<float> const& vis,
                                   const bsta_sigma_normalizer_sptr& n_table,
                                   float min_sigma=0.01f);

     static void force_mog3_sigmas_to_value(vnl_vector_fixed<unsigned char, 8> & mog3,float sigma);

};

#endif // bsta_mog3_grey_h_
