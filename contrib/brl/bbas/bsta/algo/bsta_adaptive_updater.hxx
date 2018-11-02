// This is brl/bbas/bsta/algo/bsta_adaptive_updater.hxx
#ifndef bsta_adaptive_updater_hxx_
#define bsta_adaptive_updater_hxx_
//:
// \file
#include <iostream>
#include <limits>
#include "bsta_adaptive_updater.h"
#include "bsta_gaussian_updater.h"
#include "bsta_gaussian_stats.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// The update equations used here are based on the following
// paper that extends the Stauffer-Grimson approach.
//
// Dar-Shyang Lee. Effective gaussian mixture learning for video
// background subtraction. IEEE Transactions on Pattern Analysis
// and Machine Intelligence, 27:827-832, May 2005.


//: The update function
template <class mix_dist_>
void
bsta_mg_statistical_updater<mix_dist_>::update( mix_dist_& mix, const vector_& sample, T alpha ) const
{
  const unsigned num_components = mix.num_components();

  // prune components by mahalanobis distance
  static std::vector<T> probs;
  probs.resize(num_components,T(0));
  static std::vector<unsigned int> matched;
  matched.clear();

  for (unsigned int i=0; i<num_components; ++i) {
    obs_gaussian_& g = mix.distribution(i);
    T sqr_dist = g.sqr_mahalanobis_dist(sample);
    if (sqr_dist < gt2_) {
      matched.push_back(i);
      probs[i] = sqr_dist; // initially these are distances, not probabilities
    }
  }

  // if no matches add a new component
  if (matched.empty()) {
    this->insert(mix,sample,alpha);
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
      obs_gaussian_& g = mix.distribution(m_idx);
      g.num_observations += T(1);
      T rho =(T(1)-alpha)/g.num_observations + alpha;
      bsta_update_gaussian(g, rho, sample, min_var_);
    }
    else {
      // compute probabilites for each match
      T sum_probs = T(0);
      for (unsigned int i : matched) {
        obs_gaussian_& g = mix.distribution(i);
        probs[i] = g.dist_prob_density(probs[i]) * mix.weight(i);
        sum_probs += probs[i];
      }
      // update each match
      for (unsigned int i : matched) {
        if (sum_probs != 0) {
          probs[i] /= sum_probs;
        }
        mix.set_weight(i,mix.weight(i)+alpha*probs[i]);
        obs_gaussian_& g = mix.distribution(i);
        g.num_observations += probs[i];
        T rho = probs[i] * ((1-alpha)/g.num_observations + alpha);
        bsta_update_gaussian(g, rho, sample, min_var_);
      }
    }
  }

  mix.sort(bsta_gaussian_fitness<gaussian_>::order);

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


//: The main function
template <class mix_dist_>
void
bsta_mg_grimson_statistical_updater<mix_dist_>::update( mix_dist_& mix,
                                                        const vector_& sample,
                                                        T alpha ) const
{
  int match = -1;
  unsigned int mix_nc = mix.num_components();
  for (unsigned int i=0; i<mix_nc; ++i) {
    obs_gaussian_& g = mix.distribution(i);
    T weight = (T(1)-alpha) * mix.weight(i);
    if (match<0 && g.sqr_mahalanobis_dist(sample) < gt2_) {
      weight += alpha;
      g.num_observations += T(1);
      T rho = (T(1)-alpha)/g.num_observations + alpha;
      bsta_update_gaussian(g, rho, sample, min_var_);
      match = i;
    }
    mix.set_weight(i, weight);
  }
  if (match<0) {
    this->insert(mix,sample,alpha);
    match = mix.num_components()-1;
  }

  if (match>0)
    mix.sort(bsta_gaussian_fitness<gaussian_>::order, match);
}


#define BSTA_ADAPTIVE_UPDATER_INSTANTIATE(T) \
template class bsta_mg_statistical_updater<T >; \
template class bsta_mg_grimson_statistical_updater<T >


#endif // bsta_adaptive_updater_hxx_
