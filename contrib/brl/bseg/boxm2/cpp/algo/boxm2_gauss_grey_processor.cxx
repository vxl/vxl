#include <cmath>
#include <iostream>
#include <algorithm>
#include "boxm2_gauss_grey_processor.h"
//:
// \file
#define TMATCH 2.5f

#include <boxm2/boxm2_util.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_sf1.h>

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/algo/bsta_fit_gaussian.h>

float boxm2_gauss_grey_processor::expected_color( vnl_vector_fixed<unsigned char, 2> apm)
{
  return apm[0]/255.0f;
}

float boxm2_gauss_grey_processor::gauss_prob_density(float x, float mu, float sigma)
{
  return 0.398942280f*std::exp(-0.5f*(x - mu)*(x - mu)/(sigma*sigma))/sigma;
}

float boxm2_gauss_grey_processor::prob_density(const vnl_vector_fixed<unsigned char, 2> & app, float x)
{
  float mu=((float)app[0]/255.0f);float sigma=((float)app[1]/255.0f);
  if (sigma > 0.0f)
    return gauss_prob_density(x, mu, sigma);
  return 1.0f;
}

float sigma_norm_factor(unsigned int nobs)
{
  static const float unbias_const[] = {0, 1e3f, 7.9057f, 3.0787f, 2.2646f, 1.9389f, 1.7617f, 1.6496f, 1.5716f, 1.5140f, 1.4693f, 1.4336f, 1.4043f, 1.3797f, 1.3587f, 1.3406f, 1.3248f, 1.3107f, 1.2983f, 1.2871f, 1.2770f, 1.2678f, 1.2594f, 1.2517f, 1.2446f, 1.2380f, 1.2319f, 1.2262f, 1.2209f, 1.2159f, 1.2112f, 1.2068f, 1.2026f, 1.1987f, 1.1949f, 1.1914f };

  if (nobs < 2) {
    return unbias_const[1];
  }

  if (nobs < 36) {
    return unbias_const[nobs];
  }
  // else nobs >= 36
  // approximate for big n with function a = m /nobs + b
  static const float m = (unbias_const[35] - unbias_const[30])/(1.0f/35.0f - 1.0f/30.0f);
  static const float b = unbias_const[35]  - m*(1.0f/35.0f);
  return m/nobs + b;
}

void compute_gaussian_params(std::vector<float> const& obs, std::vector<float> const& weights, float &mean, float &sigma)
{
  const unsigned int nobs = obs.size();
  double w_sum = 0.0;
  double w2_sum = 0.0;
  double obs_sum = 0.0;

  for (unsigned int i=0; i<nobs; ++i) {
    w_sum += weights[i];
    w2_sum += weights[i]*weights[i];
    obs_sum += obs[i] * weights[i];
  }
  double mean_obs = 0.5f;
  if (w_sum > 0) {
    mean_obs = obs_sum / w_sum;
  }

  double var_sum = 0.0;
  for (unsigned int i=0; i<nobs; ++i) {
    const double diff = obs[i] - mean_obs;
    var_sum += diff*diff*weights[i];
  }
  double var = 1.0f;
  if (w_sum > 0) {
    var = var_sum / (w_sum - (w2_sum/w_sum));
  }

  mean = (float)mean_obs;
  sigma = (float)std::sqrt(var);
}

//:
// Most of The following piece of code is copied from boxm_mog_grey_processor::compute_appearance
//
void boxm2_gauss_grey_processor::compute_app_model(vnl_vector_fixed<unsigned char, 2> & app,
                                                   std::vector<float> const& obs,
                                                   std::vector<float> const& vis,
                                                   const bsta_sigma_normalizer_sptr&  /*n_table*/,
                                                   float min_sigma)
{
  const auto big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]

  const unsigned int nobs = obs.size();
  if (nobs == 0) {
    // zero observations. nothing to do here.
    app[0]=(unsigned char)std::floor(0.5f*255.0f);
    app[1]=(unsigned char)std::floor(1.0f*255.0f);
    return;
  }

  if (nobs == 1) {
    // one observation: Just return the value as the mean, and a big sigma.
    app[0]=(unsigned char)std::floor(obs[0]*255.0f);
    app[1]=(unsigned char)std::floor(big_sigma*255.0f);
    return;
  }
  else {
    // compute estimate of gaussian parameters
    // Initialize the estimates
    float mean_est = 0.0f;
    float sigma_est = 0.0f;

    compute_gaussian_params(obs, vis, mean_est, sigma_est);

    sigma_est *= sigma_norm_factor(nobs);

    //float unbias_factor = n_table->normalization_factor_int(nobs);
    //sigma_est *= unbias_factor;

    // make sure standard deviation is not too small
    if (sigma_est < min_sigma) {
      sigma_est = min_sigma;
    }
    // or too big
    if (sigma_est > big_sigma) {
      sigma_est = big_sigma;
    }
    app[0]=(unsigned char)std::floor(mean_est*255.0f);
    app[1]=(unsigned char)std::floor(sigma_est*255.0f);
  }

  return;
}

void boxm2_gauss_grey_processor::compute_app_model(vnl_vector_fixed<unsigned char, 2> & apm,
                                                   std::vector<float> const& obs,
                                                   std::vector<float> const& pre,
                                                   std::vector<float> const& vis,
                                                   const bsta_sigma_normalizer_sptr& n_table,
                                                   float min_sigma)
{
  bsta_gauss_sf1 model_bsta(apm[0]/255.0f,(apm[1]/255.0f)*(apm[1]/255.0f));

  // initialize from scratch in every iteration
  //bsta_gauss_sf1 model_bsta(0.5f,(0.3f)*(0.3f));
  const auto big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]
  unsigned int nobs = obs.size();
  // check for some simple cases first
  if (nobs == 0) {
    // zero observations. nothing to do here.
    apm[0]=(unsigned char)std::floor(0.5f*255.0f);
    apm[1]=(unsigned char)std::floor(big_sigma*255.0f);
    return;
  }
  if (nobs == 1) {
    // one observation: Just return the value as the mean, and a big sigma.
    apm[0]=(unsigned char)std::floor(obs[0]*255.0f);
    apm[1]=(unsigned char)std::floor(big_sigma*255.0f);
    return;
  }
  //std::cout << "nobs = " << obs.size() << std::endl;
  //for (unsigned int i=0; i<obs.size(); ++i) {
  //  std::cout << "obs=" << obs[i] << " vis=" << vis[i] << "pre=" << pre[i] << std::endl;
  //}
  const float min_var_EM = 1.5e-5f; // to prevent degenerate solution (corresponds roughly to sigma = 1/255)
  bsta_fit_gaussian(obs,vis,pre,model_bsta,min_var_EM);
  // compute expected number of observations
  float expected_nobs = 0.0f;
  for (float vi : vis) {
    expected_nobs += vi;
  }
  // normalize sigma
  const float norm_factor = n_table->normalization_factor(expected_nobs);
  //const float norm_factor = 1.0f;
  float sigma = std::sqrt(model_bsta.var()) * norm_factor;

  // bounds check on std. deviation value
  if (sigma < min_sigma) {
    sigma = min_sigma;
  }
  if (sigma > big_sigma) {
    sigma = big_sigma;
  }
  if (!(sigma < big_sigma) && !(sigma > min_sigma)) {
    std::cerr << "error: sigma = " << sigma << " model_bsta.var() = " << model_bsta.var() << std::endl;
    sigma = big_sigma;
  }
  // convert back
  if (model_bsta.mean() <= 1.0f && model_bsta.mean() >= 0.0f) {
    apm[0]=(unsigned char)std::floor(model_bsta.mean()*255.0f);
    apm[1]=(unsigned char)std::floor(sigma*255.0f);
  }
  else {
    apm[0]=(unsigned char)std::floor(0.5f*255.0f);
    apm[1]=(unsigned char)std::floor(big_sigma*255.0f);
  }
}

void update_gauss(float & x, float & rho, float & mu, float &  sigma,float min_sigma)
{
  float var = sigma*sigma;
  float diff = x-mu;
  var = (1.0f-rho)*(var +rho*diff*diff);
  mu += rho*diff;
  sigma = std::sqrt(var);
  sigma = sigma < min_sigma ? min_sigma: sigma;
}

void boxm2_gauss_grey_processor::update_app_model(vnl_vector_fixed<unsigned char, 2> & apm,
                                                  vnl_vector_fixed<float, 4> & nobs,
                                                  float x, float w, float  /*init_sigma*/,float min_sigma)
{
  float mu = apm[0]/255.0f;
  float sigma = apm[1]/255.0f;
  update_gauss(x,w,mu, sigma, min_sigma);
  apm[0] = (unsigned char)std::floor(mu*255.0f);
  apm[1] = (unsigned char)std::floor(sigma*255.0f);
  nobs[0] += 1;
}
