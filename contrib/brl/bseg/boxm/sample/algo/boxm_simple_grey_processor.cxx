#include "boxm_simple_grey_processor.h"
//:
// \file

#include <vnl/vnl_erf.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/algo/bsta_fit_gaussian.h>

#include "boxm_sigma_normalizer.h"

//const float boxm_simple_grey_processor::one_over_sigma_ = 25.0f;
const static bool USE_UNIFORM_COMPONENT = false;

//: Return probability density of observing pixel values
float boxm_simple_grey_processor::prob_density(apm_datatype const& appear, obs_datatype const& obs)
{
  const auto norm =  float(appear.one_over_sigma() * vnl_math::one_over_sqrt2pi);
  const float diff = obs - appear.color();
  const float p = norm * std::exp(-(diff*diff)*appear.one_over_sigma()*appear.one_over_sigma()*0.5f);
  // normalize by area of distribution between 0 and 1
  //return p / total_prob(appear);
  return p * appear.gauss_weight() + (1.0f - appear.gauss_weight());
}

//: Return probabilities that pixels are in range [min,max]
float boxm_simple_grey_processor::prob_range(apm_datatype const& appear, obs_datatype const& obs_min, obs_datatype const& obs_max)
{
  // make sure obs_min and obs_max are within bounds
  const float obs_min_n = obs_min >= 0 ? obs_min : 0.0f;
  const float obs_max_n = obs_max <= 1.0? obs_max : 1.0f;

  const float diff_low = obs_min_n - appear.color();
  const float diff_high = obs_max_n - appear.color();
  const auto norm = float(appear.one_over_sigma()*vnl_math::sqrt1_2);

  const auto double_cdf_low_minus_1 = (float)vnl_erf(diff_low*norm);
  const auto double_cdf_high_minus_1 = (float)vnl_erf(diff_high*norm);
  const float P = (0.5f * (double_cdf_high_minus_1 - double_cdf_low_minus_1));
  //return P / total_prob(appear);
  return P * appear.gauss_weight() + (1.0f - appear.gauss_weight())*(obs_max - obs_min);
}

//: Return probabilities that pixels are in range [0,1] - used for normalizing
float boxm_simple_grey_processor::total_prob(apm_datatype const& appear)
{
  const float diff_low =  -appear.color();
  const float diff_high = 1.0f - appear.color();
  const auto norm = float(appear.one_over_sigma()*vnl_math::sqrt1_2);
  const auto double_cdf_low_minus_1 = (float)vnl_erf(diff_low*norm);
  const auto double_cdf_high_minus_1 = (float)vnl_erf(diff_high*norm);
  return 0.5f * (double_cdf_high_minus_1 - double_cdf_low_minus_1);
}


//: Update with a new sample image
bool boxm_simple_grey_processor::update( apm_datatype &appear, obs_datatype const& obs, float const& /*weight*/)
{
  appear = boxm_simple_grey(vnl_vector_fixed<float,3>(obs, appear.sigma(), appear.gauss_weight()));
  return true;
}


//: Expected value
boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype boxm_simple_grey_processor::expected_color(boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype const& appear)
{
  return appear.color() * appear.gauss_weight() + 0.5f * (1.0f - appear.gauss_weight());
}

//: color of the most probable mode in the mixtures in the slab
boxm_simple_grey_processor::obs_datatype boxm_simple_grey_processor::most_probable_color(apm_datatype const& appear)
{
  return appear.color();
}

void boxm_simple_grey_processor::compute_appearance(std::vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs, std::vector<float> const& pre, std::vector<float> const& vis, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model, float min_sigma)
{
  bsta_gauss_sf1 model_bsta(model.color(),model.sigma()*model.sigma());
  const auto big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]
  auto nobs = (unsigned int)obs.size();
  // check for some simple cases first
  if (nobs == 0) {
    // zero observations. nothing to do here.
    model = boxm_simple_grey(0.5f, big_sigma);
    return;
  }
  if (nobs == 1) {
    // one observation: Just return the value as the mean, and a big sigma.
    model = boxm_simple_grey(obs[0], big_sigma);
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
  static const boxm_sigma_normalizer sigma_norm(0.20f);
  const float norm_factor = sigma_norm.normalization_factor(expected_nobs);
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
  // convert back to a boxm_simple_grey model
  model = boxm_simple_grey(model_bsta.mean(),sigma);
  return;
}

void boxm_simple_grey_processor::compute_appearance(std::vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs, std::vector<float> const& weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model, float min_sigma)
{
  const auto big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]

  const auto nobs = (unsigned int)obs.size();
  if (nobs == 0) {
    // zero observations. nothing to do here.
    model = boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype(0.5f, 1.0f, 0.0f);
    return;
  }
  if (nobs == 1) {
    // one observation: Just return the value as the mean, and a big sigma.
    model = boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype(obs[0], big_sigma, weights[0]);
    return;
  }
  else {
    // compute estimate of gaussian parameters
    // Initialize the estimates
    boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype mean_est(0);
    float sigma_est = 0.0f;

    compute_gaussian_params(obs, weights, mean_est, sigma_est);
    boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype gauss_model(mean_est, sigma_est, 1.0f);

    sigma_est *= sigma_norm_factor(nobs);

    // make sure standard deviation is not too small
    if (sigma_est < min_sigma) {
      sigma_est = min_sigma;
    }
    // or too big
    if (sigma_est > big_sigma) {
      sigma_est = big_sigma;
    }

    // compute estimate of gaussian weight by summing probabilities
    float gauss_weight = 0.0f;
    if (USE_UNIFORM_COMPONENT) {
      float weight_sum = 0.0f;
      for (unsigned int n=0; n<nobs; ++n) {
        const float p_gauss = boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_processor::prob_density(gauss_model,obs[n]);
        const float p_uniform = 1.0f;
        gauss_weight += weights[n] * p_gauss / (p_gauss + p_uniform);
        weight_sum += weights[n];
      }
      if (weight_sum > 0.0f) {
        gauss_weight /= weight_sum;
      }
    }
    else {
      gauss_weight = 1.0f;
    }

    model = boxm_simple_grey(mean_est, sigma_est, gauss_weight);
  }
}


void boxm_simple_grey_processor::update_appearance(std::vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs, std::vector<float> const& weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model, float min_sigma)
{
  const auto big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]

  const auto nobs = (unsigned int)obs.size();
  if (nobs == 0) {
    // zero observations. nothing to do here.
    return;
  }
  if (nobs == 1) {
    // one observation: Just return the value as the mean, and a big sigma.
    model = boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype(obs[0], big_sigma);
    return;
  }
  else {
    // compute estimate of gaussian weight by summing probabilities
    std::vector<float> obs_gauss_weights = weights;
    float gauss_weight = 0.0f;
    float expected_nobs = 0.0f;
    if (USE_UNIFORM_COMPONENT) {
      float weight_sum = 0.0f;

      for (unsigned int n=0; n<nobs; ++n) {
        const float p_total = boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_processor::prob_density(model,obs[n]);
        const float p_uniform = 1.0f - model.gauss_weight();
        const float p_gauss = p_total - p_uniform;
        if (p_total > 0.0f) {
          obs_gauss_weights[n] = weights[n] * p_gauss / p_total;
        }
        else {
          obs_gauss_weights[n] = 0.0f;
        }
        gauss_weight += obs_gauss_weights[n];
        weight_sum += weights[n];
      }
      if (weight_sum > 0.0f) {
        gauss_weight /= weight_sum;
      }
      expected_nobs = weight_sum;
    }
    else {
      auto wit = weights.begin();
      for (; wit != weights.end(); ++wit) {
        expected_nobs += *wit;
      }
      gauss_weight = 1.0f;
    }
    // Initialize the estimates
    boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype mean_est(0);
    float sigma_est = 0.0f;

    compute_gaussian_params(obs, obs_gauss_weights, mean_est, sigma_est);

    //sigma_est *= sigma_norm_factor(nobs);

    // make sure standard deviation is not too small
    if (sigma_est < min_sigma) {
      sigma_est = min_sigma;
    }
    // or too big
    if (sigma_est > big_sigma) {
      sigma_est = big_sigma;
    }

    model = boxm_simple_grey(mean_est, sigma_est, gauss_weight);
  }
  return;
}

void boxm_simple_grey_processor::finalize_appearance(std::vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs,
                                                     std::vector<float> const& weights,
                                                     boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model)
{
  const auto big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]

  float expected_nobs = 0.0f;
    const auto nobs = (unsigned int)obs.size();

  auto wit = weights.begin();
  for (; wit != weights.end(); ++wit) {
    expected_nobs += *wit;
  }

  //float unbiased_sigma = model.sigma() * sigma_norm_factor(expected_nobs);
  float unbiased_sigma = model.sigma() * sigma_norm_factor(nobs);
  if (unbiased_sigma > big_sigma) {
    unbiased_sigma = big_sigma;
  }


  model = boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype(model.color(), unbiased_sigma, model.gauss_weight());
}


void boxm_simple_grey_processor::compute_gaussian_params(std::vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> obs, std::vector<float> weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype &mean, float &sigma)
{
  const auto nobs = (unsigned int)obs.size();
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

float boxm_simple_grey_processor::sigma_norm_factor(float nobs)
{
  if (nobs <= 1.0f) {
    return sigma_norm_factor((unsigned int)1);
  }

  // linearly interpolate between integer values
  float nobs_floor = std::floor(nobs);
  float nobs_ceil = std::ceil(nobs);
  float floor_weight = nobs_ceil - nobs;
  float norm_factor = (sigma_norm_factor((unsigned int)nobs_floor) * floor_weight) + (sigma_norm_factor((unsigned int)nobs_ceil) * (1.0f - floor_weight));

  return norm_factor;
}

float boxm_simple_grey_processor::sigma_norm_factor(unsigned int nobs)
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

void boxm_simple_grey_processor::
boxm_compute_shadow_appearance(std::vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs,
                               std::vector<float> const& pre,
                               std::vector<float> const& vis,
                               boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model,
                               float min_app_sigma,
                               float shadow_prior,
                               float shadow_mean,
                               float shadow_sigma,
                               bool verbose )
{
  std::vector<float> obs_weights(vis.size(), 0.0f);
  std::vector<float> sh_density(vis.size(), 0.0f);
  //assume that the initial appearance distribution is uniform on [0 1]
  //thus a reasonable initialization for weights is
  //
  //  vis(1-Psh)/( (1-Psh) + Psh psh(obs) )
  //
  //, where Psh is shadow_prior and psh(x) is the shadow density function,
  // sh_density. vis is the visibilty probability
  //
  boxm_simple_grey shadow_dist(shadow_mean, shadow_sigma);
  for (unsigned n = 0; n<obs.size(); ++n)
  {
    //2.0 x density since centered on zero
    sh_density[n] =
      2.0f*boxm_simple_grey_processor::prob_density(shadow_dist, obs[n]);
    float neu = vis[n]*(1-shadow_prior);
    float weight =
      neu/(neu + sh_density[n]*shadow_prior);
    obs_weights[n]=weight;
  }
  std::vector<float> initial_weights = obs_weights;
  // initial estimate for the illuminated appearance model
  boxm_simple_grey_processor::compute_appearance(obs,obs_weights,model,min_app_sigma);
  double initial_mean = model.color();
  double initial_sigma = model.sigma();

  // use EM to refine estimate until convergence.
  auto nobs = (unsigned int)obs.size();
  if (nobs == 0) {
    // nothing to do.
    return;
  }
  const float epsilon = 1e-4f;
  constexpr unsigned int max_its = 100;
  const float min_weight_change = 1e-5f;
  unsigned int i=0;
  for (; i<max_its; ++i) {
    float max_weight_change = 0.0f;
    // EXPECTATION - determine the weights
    for (unsigned int n=0; n<nobs; ++n) {
      float PI = vis[n] * boxm_simple_grey_processor::prob_density(model,obs[n]);
      float neu = PI*(1-shadow_prior);
      float den = (PI + pre[n])*(1-shadow_prior) + shadow_prior*sh_density[n];
      float new_obs_weight = 0;
      if (den > epsilon) {
        new_obs_weight = neu / den;
      }
      // compute delta weight for convergence check
      float weight_delta = std::fabs(obs_weights[n] - new_obs_weight);
      if (weight_delta > max_weight_change) {
        max_weight_change = weight_delta;
      }
      obs_weights[n] = new_obs_weight;
    }
    // check for convergence
    if (max_weight_change < min_weight_change)
      break;
    // MAXIMIZATION - determine the parameters of the appearance distribution
    boxm_simple_grey_processor::update_appearance(obs,obs_weights,model,min_app_sigma);
  }
  if (verbose) {
    bool print = false;
    for (unsigned n = 0; n<obs.size(); ++n) {
      if (obs[n]>0) print = true;
      if (i<max_its&&print) {
        std::cout << "converged after " << i << " iterations.\n"
                 << " initial mean = " << initial_mean << " final mean = "
                 << model.color() << '\n'
                 << " initial sigma = " << initial_sigma << " final sigma = "
                 << model.sigma() << '\n';
      }
      else { std::cout << "failed to converge\n"; }

      std::cout << "Initial (Observations, Weights):\n";
      for (unsigned n=0; n<obs.size(); ++n)
        std::cout << '('<< obs[n] << ' ' << initial_weights[n] << ") ";
      std::cout << "\nFinal (Observations, Weights):\n";
      for (unsigned n=0; n<obs.size(); ++n)
        std::cout << '('<< obs[n] << ' ' << obs_weights[n] << ") ";
      std::cout << '\n';
    }
  }

  // adjust model to account for small sample sizes.
  boxm_simple_grey_processor::finalize_appearance(obs,obs_weights,model);
}
