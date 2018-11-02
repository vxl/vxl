#ifndef bsta_beta_updater_hxx_
#define bsta_beta_updater_hxx_
//:
// \file
#include <iostream>
#include <limits>
#include "bsta_beta_updater.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: The update function
template <class mix_dist_>
void
bsta_mix_beta_updater<mix_dist_>::update( mix_dist_& mix, const vector_& sample, T alpha ) const
{
  unsigned num_components = mix.num_components();

  // prune components by probability distribution threshold
  static std::vector<T> probs;
  probs.resize(num_components,T(0));
  static std::vector<unsigned int> matched;
  matched.clear();

  for (unsigned int i=0; i<num_components; ++i) {
    obs_dist_& d = mix.distribution(i);
    T p = d.prob_density(sample);
    T dist_p=d.distance(sample);
    if (dist_p > -p_thresh_){
      matched.push_back(i);
      probs[i] = p; // ???? SHOULD BE p ?? initially these are distances, not probabilities
    }
  }

  // if matches are not good add a new component
  if (matched.empty()) {
    insert(mix,sample,alpha);
    mix.normalize_weights();
  }
  else
  {
    // update the weights
    for (unsigned int i=0; i<num_components; ++i) {
      T weight = (T(1)-alpha) * mix.weight(i);
      mix.set_weight(i,weight);
    }
    // special case of 1 match - don't need to compute probabilites
    if (matched.size() == 1) {
      unsigned int m_idx = matched.front();
      mix.set_weight(m_idx,mix.weight(m_idx)+alpha);
      obs_dist_& b = mix.distribution(m_idx);
      b.num_observations += T(1);
      T rho =(T(1)-alpha)/b.num_observations + alpha;
      bsta_update_beta(b, rho, sample);
    }
    else {
      // compute probabilites for each match
      T sum_probs = T(0);
      for (unsigned int i : matched) {
        // obs_dist_& b = mix.distribution(i);
        probs[i] *= mix.weight(i); //?????? b.dist_prob_density(probs[i]) * mix.weight(i);
        sum_probs += probs[i];
      }
      // update each match
      for (unsigned int i : matched) {
        if (sum_probs != 0) {
          probs[i] /= sum_probs;
        }
        mix.set_weight(i,mix.weight(i)+alpha*probs[i]);
        obs_dist_& b = mix.distribution(i);
        b.num_observations += probs[i];
        T rho = probs[i] * ((1-alpha)/b.num_observations + alpha);
        bsta_update_beta(b, rho, sample);
      }
    }
  }

  mix.sort(bsta_beta_fitness<dist_>::order);

  // try to clean up gaussian components with weights that have converged to zero
  if (mix.weight(mix.num_components()-1) < std::numeric_limits<T>::epsilon()) {
    mix.remove_last();
    T sum = 0;
    for (unsigned int i=0; i<mix.num_components(); ++i) {
      sum += mix.weight(i);
    }
    std::cout << "removed, total weight = " << sum << std::endl;
    mix.normalize_weights();
  }
}

#define BSTA_MIX_BETA_UPDATER_INSTANTIATE(T) \
template class bsta_mix_beta_updater<T >

#endif
