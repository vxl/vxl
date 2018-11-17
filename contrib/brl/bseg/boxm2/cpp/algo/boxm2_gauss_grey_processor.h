// This is brl/bseg/boxm2/cpp/algo/boxm2_gauss_grey_processor.h
#ifndef boxm2_gauss_grey_processor_h_
#define boxm2_gauss_grey_processor_h_
//:
// \file
// \brief A class for a grey-scale-gaussian processor
//
// \author Ozge C. Ozcanli
// \date   June 15, 2011
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <vector>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bsta/algo/bsta_sigma_normalizer.h>

class  boxm2_gauss_grey_processor
{
 public:
     static float expected_color( vnl_vector_fixed<unsigned char, 2> apm);
     static float prob_density( const vnl_vector_fixed<unsigned char, 2> & apm, float x);
     static float gauss_prob_density(float x, float mu, float sigma);
     static void  update_app_model(vnl_vector_fixed<unsigned char, 2> & apm,
                                        vnl_vector_fixed<float, 4> & nobs,
                                        float x, float w, float init_sigma,float min_sigma);

     static void compute_app_model(vnl_vector_fixed<unsigned char, 2> & apm,
                                         std::vector<float> const& obs,
                                         std::vector<float> const& obs_weights,
                                         const bsta_sigma_normalizer_sptr& n_table,
                                         float min_sigma = 0.01f);

     static void compute_app_model(vnl_vector_fixed<unsigned char, 2> & apm,
                                   std::vector<float> const& obs,
                                   std::vector<float> const& pre,
                                   std::vector<float> const& vis,
                                   const bsta_sigma_normalizer_sptr& n_table,
                                   float min_sigma=0.01f);
};

#endif // boxm2_gauss_grey_processor_h_
