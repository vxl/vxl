#ifndef boxm_shadow_appearance_estimator_h_
#define boxm_shadow_appearance_estimator_h_

#include <vector>
#include <iostream>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#if 1
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

template <boxm_apm_type APM>
void boxm_compute_shadow_appearance(std::vector<typename boxm_apm_traits<APM>::obs_datatype> const& obs,
                                    std::vector<float> const& pre,
                                    std::vector<float> const& vis,
                                    typename boxm_apm_traits<APM>::apm_datatype &model,
                                    float min_app_sigma,
                                    float shadow_prior,
                                    float shadow_mean, float shadow_sigma,
                                    bool verbose = false)
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
  for(unsigned n = 0; n<obs.size(); ++n)
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
  boxm_apm_traits<APM>::apm_processor::compute_appearance(obs,obs_weights,model,min_app_sigma);
  double initial_mean = model.color();
  double initial_sigma = model.sigma();
#if 1
  // use EM to refine estimate until convergence.
  unsigned int nobs= obs.size();
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

      float PI = vis[n] * boxm_apm_traits<APM>::apm_processor::prob_density(model,obs[n]);
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
    boxm_apm_traits<APM>::apm_processor::update_appearance(obs,obs_weights,model,min_app_sigma);
  }
  if(verbose) {
    bool print = false;
    for(unsigned n = 0; n<obs.size(); ++n)
      if(obs[n]>0) print = true;
      if(i<max_its&&print){
        std::cout << "converged after " << i << " iterations.\n";
        std::cout << " initial mean = " << initial_mean << " final mean = "
                 << model.color() << '\n';
        std::cout << " initial sigma = " << initial_sigma << " final sigma = "
                 << model.sigma() << '\n';
      }else{ std::cout << "failed to converge\n";}

      std::cout << "Initial (Observations, Weights):\n";
      for(unsigned n=0; n<obs.size(); ++n)
        std::cout << '('<< obs[n] << ' ' << initial_weights[n] << ") ";
      std::cout << '\n';
      std::cout << "Final (Observations, Weights):\n";
      for(unsigned n=0; n<obs.size(); ++n)
        std::cout << '('<< obs[n] << ' ' << obs_weights[n] << ") ";
      std::cout << '\n';
  }

  // adjust model to account for small sample sizes.
  boxm_apm_traits<APM>::apm_processor::finalize_appearance(obs,obs_weights,model);
#endif
  return;
}

#endif // boxm_shadow_appearance_estimator_h_
