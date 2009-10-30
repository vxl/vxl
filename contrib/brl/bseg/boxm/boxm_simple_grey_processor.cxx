#include "boxm_simple_grey_processor.h"
//:
// \file

#include <vnl/vnl_erf.h>

//const float boxm_simple_grey_processor::one_over_sigma_ = 25.0f;
const static bool USE_UNIFORM_COMPONENT = false;

//: Return probability density of observing pixel values
float boxm_simple_grey_processor::prob_density(apm_datatype const& appear, obs_datatype const& obs)
{
  const float norm =  float(appear.one_over_sigma() * vnl_math::one_over_sqrt2pi);
  const float diff = obs - appear.color();
  const float p = norm * vcl_exp(-(diff*diff)*appear.one_over_sigma()*appear.one_over_sigma()*0.5f);
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
  const float norm = float(appear.one_over_sigma()*vnl_math::sqrt1_2);

  const float double_cdf_low_minus_1 = (float)vnl_erf(diff_low*norm);
  const float double_cdf_high_minus_1 = (float)vnl_erf(diff_high*norm);
  const float P = (0.5f * (double_cdf_high_minus_1 - double_cdf_low_minus_1));
  //return P / total_prob(appear);
  return P * appear.gauss_weight() + (1.0f - appear.gauss_weight())*(obs_max - obs_min);
}

//: Return probabilities that pixels are in range [0,1] - used for normalizing
float boxm_simple_grey_processor::total_prob(apm_datatype const& appear)
{
  const float diff_low =  -appear.color();
  const float diff_high = 1.0f - appear.color();
  const float norm = float(appear.one_over_sigma()*vnl_math::sqrt1_2);
  const float double_cdf_low_minus_1 = (float)vnl_erf(diff_low*norm);
  const float double_cdf_high_minus_1 = (float)vnl_erf(diff_high*norm);
  return 0.5f * (double_cdf_high_minus_1 - double_cdf_low_minus_1);
}


//: Update with a new sample image
bool boxm_simple_grey_processor::update( apm_datatype &appear, obs_datatype const& obs, float const& weight)
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


void boxm_simple_grey_processor::compute_appearance(vcl_vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs, vcl_vector<float> const& weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model, float min_sigma)
{
  const float big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]

  const unsigned int nobs = obs.size();
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


void boxm_simple_grey_processor::update_appearance(vcl_vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs, vcl_vector<float> const& weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model, float min_sigma)
{
  const float big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]

  const unsigned int nobs = obs.size();
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
    vcl_vector<float> obs_gauss_weights = weights;
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
        } else {
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
      vcl_vector<float>::const_iterator wit = weights.begin();
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

void boxm_simple_grey_processor::finalize_appearance(vcl_vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> const& obs,
                                                     vcl_vector<float> const& weights,
                                                     boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype &model)
{
  const float big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]

  float expected_nobs = 0.0f;
    const unsigned int nobs = obs.size();

  vcl_vector<float>::const_iterator wit = weights.begin();
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


void boxm_simple_grey_processor::compute_gaussian_params(vcl_vector<boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype> obs, vcl_vector<float> weights, boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype &mean, float &sigma)
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
  sigma = (float)vcl_sqrt(var);
}

float boxm_simple_grey_processor::sigma_norm_factor(float nobs)
{
  if (nobs <= 1.0f) {
    return sigma_norm_factor((unsigned int)1);
  }

  // linearly interpolate between integer values
  float nobs_floor = vcl_floor(nobs);
  float nobs_ceil = vcl_ceil(nobs);
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

