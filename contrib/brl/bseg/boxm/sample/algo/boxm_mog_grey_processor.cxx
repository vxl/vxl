#include "boxm_mog_grey_processor.h"
//:
// \file
#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/algo/bsta_fit_gaussian.h>

#include <vnl/vnl_random.h>

#include "boxm_sigma_normalizer.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Expected value
boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype
boxm_mog_grey_processor::expected_color(boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype const& appear)
{
  float total_weight= 0;
  obs_datatype c = 0;
  obs_datatype expected = 0;

  //should be components used
  for (unsigned i = 0; i< appear.num_components(); ++i)
  {
    total_weight += appear.weight(i);
    c += appear.distribution(i).mean() * appear.weight(i);
  }
  if (total_weight > 0.0f) {
    expected = c/total_weight;
  }

  return expected;
}

//: Return a random sample from the appearance distribution
boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype
boxm_mog_grey_processor::sample(boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype const& appear, vnl_random &rand_gen)
{
  return appear.sample(rand_gen);
}


//: Return probability density of observing pixel values
float boxm_mog_grey_processor::prob_density(apm_datatype const& appear, obs_datatype const& obs)
{
  if (appear.num_components() ==0) {
    return 1.00f;
  }
  else {
    return appear.prob_density(obs);
  }
}

//: Update with a new sample image
bool boxm_mog_grey_processor::update( apm_datatype &appear, obs_datatype const& obs, float const& weight)
{
  // the model
  float init_variance = 0.008f;
  float min_stddev = 0.030f; //TO UNDERSTAND: changed from 0.02 for dinoRing dataset -dec
  float g_thresh = 2.5; // number of std devs from mean sample must be
  bsta_gauss_sf1 this_gauss(0.0f, init_variance);

  const unsigned int nmodes = boxm_apm_traits<BOXM_APM_MOG_GREY>::n_gaussian_modes_;

  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, nmodes> mix_gauss;

  // the updater
  bsta_mg_grimson_weighted_updater<mix_gauss> updater(this_gauss, nmodes ,g_thresh,min_stddev);

  if (weight > 0) {
    updater(appear, obs, weight);
  }

  return true;
}

void boxm_mog_grey_processor::update_appearance(std::vector<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> const& obs, std::vector<float> const& obs_weights, boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype &model, float min_sigma)
{
  std::vector<float> pre(obs.size(),0.0f);
  compute_appearance(obs,pre,obs_weights,model,min_sigma);
  return;
}

void boxm_mog_grey_processor::compute_appearance(std::vector<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> const& obs, std::vector<float> const& obs_weights, boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype &model, float min_sigma)
{
  std::vector<float> pre(obs.size(),0.0f);
  compute_appearance(obs,pre,obs_weights,model,min_sigma);
  return;
}

void boxm_mog_grey_processor::finalize_appearance(std::vector<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> const& obs,
                                                  std::vector<float> const&  /*obs_weights*/, // FIXME - unused
                                                  boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype &model,
                                                  float min_sigma)
{
  static const unsigned int nmodes = boxm_apm_traits<BOXM_APM_MOG_GREY>::n_gaussian_modes_;
  const auto nobs = (unsigned int)obs.size();
  const float min_var = min_sigma*min_sigma;
  const auto big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]
  const float big_var = big_sigma * big_sigma;

  static boxm_sigma_normalizer sigma_norm(0.1f);
  for (unsigned int m=0; m<nmodes; ++m) {
    //float unbias_factor = sigma_norm.normalization_factor(mode_weight_sum[m]);
    float unbias_factor = sigma_norm.normalization_factor(model.weight(m) * nobs);

    //float unbias_factor = sigma_norm.normalization_factor_int(nobs);
    float mode_var = model.distribution(m).var();
    mode_var *= (unbias_factor*unbias_factor);

    // make sure variance does not get too big
    if (!(mode_var < big_var)) {
      mode_var = big_var;
    }
    // or too small
    if (!(mode_var > min_var)) {
      mode_var = min_var;
    }
    model.distribution(m).set_var(mode_var);
  }
  return;
}

void boxm_mog_grey_processor::compute_appearance(std::vector<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> const& obs, std::vector<float> const& pre, std::vector<float> const& vis, boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype &model, float min_sigma)
{
  static const unsigned int max_nmodes = boxm_apm_traits<BOXM_APM_MOG_GREY>::n_gaussian_modes_;
  const float min_var = min_sigma*min_sigma;
  const auto big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]
  const float big_var = big_sigma * big_sigma;

  auto nobs = (unsigned int)obs.size();
  if (nobs == 0) {
    // nothing to do.
    return;
  }
  if (nobs == 1) {
    model = bsta_num_obs<bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, max_nmodes> >();
    bsta_gauss_sf1 mode(obs[0], big_sigma*big_sigma);
    model.insert(bsta_num_obs<bsta_gauss_sf1>(mode), 1.0f);
    return;
  }

  unsigned int nmodes = model.num_components();

  if (nmodes == 0) {
    // if initial model is "blank", create an arbitrary starting point for the optimization with max number of modes
    model = bsta_num_obs<bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, max_nmodes> >();
    nmodes = max_nmodes;
    // no need for model to have more modes than observations
    if (nobs < nmodes) {
      nmodes = nobs;
    }
    // initialize parameters
    for (unsigned int m=0; m<nmodes; ++m) {
      float mean = (float(m) + 0.5f) / float(nmodes);
      float sigma = 0.3f;
      float mode_weight = 1.0f / float(nmodes);
      bsta_gauss_sf1 mode(mean, sigma*sigma);
      model.insert(bsta_num_obs<bsta_gauss_sf1>(mode), mode_weight);
    }
  }

  std::vector<std::vector<float> > mode_probs(nobs);
  for (unsigned int n=0; n<nobs; ++n) {
    for (unsigned int m=0; m<nmodes; ++m) {
      mode_probs[n].push_back(0.0f);
    }
  }
  std::vector<float> mode_weight_sum(nmodes,0.0f);

  // run EM algorithm to maximize expected probability of observations
  constexpr unsigned int max_its = 50;
  const float max_converged_weight_change = 1e-3f;

  for (unsigned int i=0; i<max_its; ++i) {
    float max_weight_change = 0.0f;
    // EXPECTATION
    for (unsigned int n=0; n<nobs; ++n) {
      // for each observation, assign probabilities to each mode of appearance model (and to a "previous cell")
      float total_prob = 0.0f;
      std::vector<float> new_mode_probs(nmodes);
      for (unsigned int m=0; m<nmodes; ++m) {
        // compute probability that nth data point was produced by mth mode
        const float new_mode_prob = vis[n] * model.distribution(m).prob_density(obs[n]) * model.weight(m);
        new_mode_probs[m] = new_mode_prob;
        total_prob += new_mode_prob;
      }
      // compute the probability the observation came from an occluding cell
      const float prev_prob = pre[n];
      total_prob += prev_prob;
      if (total_prob > 1e-6) {
        for (unsigned int m=0; m<nmodes; ++m) {
          new_mode_probs[m] /= total_prob;
          const float weight_change = std::fabs(new_mode_probs[m] - mode_probs[n][m]);
          if (weight_change > max_weight_change) {
            max_weight_change = weight_change;
          }
          mode_probs[n][m] = new_mode_probs[m];
        }
      }
    }
    // check for convergence
    if (max_weight_change < max_converged_weight_change) {
      break;
    }
    // MAXIMIZATION
    // computed the weighted means and variances for each mode based on the probabilities
    float total_weight_sum = 0.0f;

    // update the mode parameters
    for (unsigned int m=0; m<nmodes; ++m) {
      mode_weight_sum[m] = 0.0f;
      std::vector<float> obs_weights(nobs);
      for (unsigned int n=0; n<nobs; ++n) {
        obs_weights[n] = mode_probs[n][m];
        mode_weight_sum[m] += obs_weights[n];
      }
      total_weight_sum += mode_weight_sum[m];
      float mode_mean(0.5f);
      float mode_var(1.0f);
      bsta_gauss_sf1 single_gauss(mode_mean,mode_var);
      bsta_fit_gaussian(obs, obs_weights, single_gauss);
      mode_mean = single_gauss.mean();
      mode_var = single_gauss.var();

      // unbias variance based on number of observations
      //float unbias_factor = sigma_norm_factor(mode_weight_sum[m]);
      // mode_var *= (unbias_factor*unbias_factor);

      // make sure variance does not get too big
      if (!(mode_var < big_var)) {
        mode_var = big_var;
      }
      // or too small
      if (!(mode_var > min_var)) {
        mode_var = min_var;
      }

      // update mode parameters
      model.distribution(m).set_mean(mode_mean);
      model.distribution(m).set_var(mode_var);
    }
    // update mode probabilities
    if (total_weight_sum > 1e-6) {
      for (unsigned int m=0; m<nmodes; ++m) {
        const float mode_weight = mode_weight_sum[m] / total_weight_sum;
        // update mode weight
        model.set_weight(m, mode_weight);
      }
    }
  }

  // unbias variance based on number of observations
  static const boxm_sigma_normalizer sigma_norm(0.20f);
  for (unsigned int m=0; m<nmodes; ++m) {
    float unbias_factor = sigma_norm.normalization_factor(mode_weight_sum[m]);
    //float unbias_factor = sigma_norm.normalization_factor(model.weight(m) * nobs);
    //float unbias_factor = sigma_norm.normalization_factor_int(nobs);

    float mode_var = model.distribution(m).var();
    mode_var *= (unbias_factor*unbias_factor);

    // make sure variance does not get too big
    if (!(mode_var < big_var)) {
      mode_var = big_var;
    }
    // or too small
    if (!(mode_var > min_var)) {
      mode_var = min_var;
    }
    model.distribution(m).set_var(mode_var);
  }

  // sort the modes based on weight
  model.sort();

  return;
}
