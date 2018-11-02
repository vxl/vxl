// This is brl/bbas/bsta/algo/bsta_fit_gaussian.h
#ifndef bsta_fit_gaussian_h_
#define bsta_fit_gaussian_h_
//:
// \file
// \brief Functions for fitting Gaussian distributions to a set of samples
// \author Daniel Crispell (daniel_crispell@brown.edu)
// \date February 9, 2010
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <iostream>
#include <bsta/bsta_gaussian.h>
#include <bsta/bsta_gaussian_sphere.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/bsta_gaussian_full.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_attributes.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: fit a 1D Gaussian distribution to a set of weighted samples
template <class T>
void bsta_fit_gaussian(std::vector<T> const& samples, std::vector<T> const& sample_weights, bsta_gaussian_sphere<T,1>& gaussian)
{
  // sanity check
  if (samples.size() != sample_weights.size()) {
    std::cerr << "bsta_fit_gaussian : error - samples.size == " << samples.size()
             << ", sample_weights.size == " << sample_weights.size() << std::endl;
    return;
  }

 const unsigned int nobs = (unsigned int)samples.size();
  T weight_sum = 0;

  // compute mean
  T mean = 0;
  for (unsigned int i=0; i<nobs; ++i) {
    mean += samples[i]*sample_weights[i];
    weight_sum += sample_weights[i];
  }
  if (weight_sum > 0) {
    mean /= weight_sum;
  }
  else {
    // error: no samples with non-zero weight!
    return;
  }

  // compute variance
  T var = 0;
  T weight_norm_sqrd_sum = 0;

  for (unsigned int i=0; i<nobs; ++i) {
    const T diff = samples[i] - mean;
    const T weight_norm = sample_weights[i] / weight_sum;
    var += diff*diff*weight_norm;
    weight_norm_sqrd_sum += weight_norm*weight_norm;
  }
  if( (T)(1-weight_norm_sqrd_sum) > (T) 0 )
    var /= ((T)1 - weight_norm_sqrd_sum);
  else
    var = (T) 0;

  gaussian.set_mean(mean);
  if(var > 0)
    gaussian.set_covar(var);
}

//: fit a N-D spherical Gaussian distribution to a set of weighted samples
template <class T, unsigned n>
void bsta_fit_gaussian(std::vector<vnl_vector_fixed<T,n> > const& samples, std::vector<T> const& sample_weights,
                       bsta_gaussian_sphere<T,n>& gaussian )
{
  // sanity check
  if (samples.size() != sample_weights.size()) {
    std::cerr << "bsta_fit_gaussian : error - samples.size == " << samples.size()
             << ", sample_weights.size == " << sample_weights.size() << std::endl;
    return;
  }

 const unsigned int nobs = samples.size();
  T weight_sum = 0;

  // compute mean
  vnl_vector_fixed<T,n> mean((T)0);
  for (unsigned int i=0; i<nobs; ++i) {
    mean += samples[i]*sample_weights[i];
    weight_sum += sample_weights[i];
  }
  if (weight_sum > 0) {
    mean /= weight_sum;
  }
  else {
    // error: no samples with non-zero weight!
    return;
  }

  // compute variance: single variance for all dimensions
  T var = 0;
  T weight_norm_sqrd_sum = 0;

  for (unsigned int i=0; i<nobs; ++i) {
    const vnl_vector_fixed<T,n> diff = samples[i] - mean;
    const T weight_norm = sample_weights[i] / weight_sum;
    var += dot_product(diff,diff)*weight_norm/n;
    weight_norm_sqrd_sum += weight_norm*weight_norm;
  }
  var /= ((T)1 - weight_norm_sqrd_sum);

  gaussian.set_mean(mean);
  gaussian.set_covar(var);
}


//: fit a N-D independent Gaussian distribution to a set of weighted samples
template <class T, unsigned n>
void bsta_fit_gaussian(std::vector<vnl_vector_fixed<T,n> > const& samples, std::vector<T> const& sample_weights,
                       bsta_gaussian_indep<T,n>& gaussian)
{
  const unsigned int nobs = samples.size();
  // sanity check
  if (nobs != sample_weights.size()) {
    std::cerr << "bsta_fit_gaussian : error - samples.size == " << samples.size()
             << ", sample_weights.size == " << sample_weights.size() << std::endl;
    return;
  }

  // compute mean
  T weight_sum = 0;
  vnl_vector_fixed<T,n> mean((T)0);
  for (unsigned int i=0; i<nobs; ++i) {
    mean += samples[i]*sample_weights[i];
    weight_sum += sample_weights[i];
  }
  if (weight_sum > 0) {
    mean /= weight_sum;
  }
  else {
    // error: no samples with non-zero weight!
    return;
  }

  // compute variance independently for each dimension
  vnl_vector_fixed<T,n> diag_covar((T)0);
  T weight_norm_sqrd_sum = 0;

  for (unsigned int i=0; i<nobs; ++i) {
    const vnl_vector_fixed<T,n> diff = samples[i] - mean;
    const T weight_norm = sample_weights[i] / weight_sum;
    diag_covar += element_product(diff,diff)*weight_norm;
    weight_norm_sqrd_sum += weight_norm*weight_norm;
  }
  diag_covar /= ((T)1 - weight_norm_sqrd_sum);

  gaussian.set_mean(mean);
  gaussian.set_covar(diag_covar);
}


//: fit a N-D Gaussian distribution with full covariance to a set of weighted samples
template <class T, unsigned n>
void bsta_fit_gaussian(std::vector<vnl_vector_fixed<T,n> > const& samples, std::vector<T> const& sample_weights,
                       bsta_gaussian_full<T,n>& gaussian)
{
  const unsigned int nobs = static_cast<const unsigned>(samples.size());
  // sanity check
  if (nobs != static_cast<unsigned>(sample_weights.size())) {
    std::cerr << "bsta_fit_gaussian : error - samples.size == " << samples.size()
             << ", sample_weights.size == " << sample_weights.size() << std::endl;
    return;
  }

  // compute mean
  T weight_sum = 0;
  vnl_vector_fixed<T,n> mean((T)0);
  for (unsigned int i=0; i<nobs; ++i) {
    mean += samples[i]*sample_weights[i];
    weight_sum += sample_weights[i];
  }
  if (weight_sum > 0) {
    mean /= weight_sum;
  }
  else {
    // error: no samples with non-zero weight!
    return;
  }

  // compute variance independently for each dimension
  vnl_matrix_fixed<T,n,n> covar((T)0);
  T weight_norm_sqrd_sum = 0;

  for (unsigned int i=0; i<nobs; ++i) {
    const vnl_vector_fixed<T,n> diff = samples[i] - mean;
    const T weight_norm = sample_weights[i] / weight_sum;
    covar += outer_product(diff,diff)*weight_norm;
    weight_norm_sqrd_sum += weight_norm*weight_norm;
  }
  covar /= ((T)1 - weight_norm_sqrd_sum);

  gaussian.set_mean(mean);
  gaussian.set_covar(covar);
}

// Helper function for clipping covariance matrix
template<class T>
T clip_covar(T var, T min_var)
{ return std::max(var,min_var); }

// Helper function for clipping covariance matrix
template<class T, unsigned n>
vnl_vector_fixed<T,n> clip_covar(vnl_vector_fixed<T,n> covar, vnl_vector_fixed<T,n> min_covar)
{
  vnl_vector_fixed<T,n> maxval;
  for (unsigned int i=0; i<n; ++i) {
    maxval[i] = std::max(covar[i],min_covar[i]);
  }
  return maxval;
}

// Helper function for clipping covariance matrix
template<class T, unsigned n>
vnl_matrix_fixed<T,n,n> clip_covar(vnl_matrix_fixed<T,n,n> covar, vnl_matrix_fixed<T,n,n> min_covar)
{
  vnl_matrix_fixed<T,n,n> maxval;
  for (unsigned int i=0; i<n; ++i) {
    for (unsigned int j=0; j<n; ++j) {
      maxval(i,j) = std::max(covar(i,j),min_covar(i,j));
    }
  }
  return maxval;
}


// Uses EM to compute mean and sigma.
// min_var prevents EM from converging to degenerate distribution centered around a single sample
template <class gauss_type>
void bsta_fit_gaussian(std::vector<typename gauss_type::vector_type> const& samples,
                       std::vector<typename gauss_type::math_type> const& sample_probs,
                       std::vector<typename gauss_type::math_type> const& prob_densities_other,
                       gauss_type& gaussian, typename gauss_type::covar_type min_covar)
{
  // sanity checks
  const unsigned int nobs = (unsigned int)samples.size();
  if (nobs != sample_probs.size()) {
    std::cerr << "bsta_fit_gaussian : error - samples.size == " << samples.size()
             << ", sample_probs.size == " << sample_probs.size() << std::endl;
    return;
  }
  if (nobs != prob_densities_other.size()) {
    std::cerr << "bsta_fit_gaussian : error - samples.size == " << samples.size()
             << ", prob_densities_other.size == " << prob_densities_other.size() << std::endl;
    return;
  }
  if (nobs == 0) {
    // zero observations. nothing to do here.
    return;
  }

  // initialize with estimate of gaussian parameters
  typedef typename gauss_type::math_type T;

  std::vector<T> sample_weights = sample_probs;
  bsta_fit_gaussian(samples, sample_weights, gaussian);

  constexpr unsigned int max_its = 100;
  const T max_converged_weight_change = 1e-4f;
  for (unsigned int i=0; i<max_its; ++i)
  {
    T max_weight_change = 0;
    T sample_weight_sum = 0;
    // EXPECTATION
    for (unsigned int n=0; n<nobs; ++n) {
      // for each observation, assign probabilities that observation was produced by this model
      // P1: observation is generated by this gaussian.
      // P2: observation is not valid - observation is generated by the alternate model
      const T P1 = sample_probs[n] * gaussian.prob_density(samples[n]);
      const T P2 = prob_densities_other[n];
      const T normalizing_factor = P1 + P2;
      T new_sample_weight = 0;
      if (normalizing_factor > 1e-6) {
        new_sample_weight = P1 / normalizing_factor;
      }
      // compute delta weight for convergence check
      T weight_delta = std::fabs(sample_weights[n] - new_sample_weight);
      if (weight_delta > max_weight_change) {
        max_weight_change = weight_delta;
      }
      sample_weights[n] = new_sample_weight;
      sample_weight_sum += new_sample_weight;
    }
    // check for convergence
    if (max_weight_change < max_converged_weight_change) {
      break;
    }
    // MAXIMIZATION
    bsta_fit_gaussian(samples, sample_weights, gaussian);
    // make sure covariance does not get too "tight" to avoid degenerate solutions
    gaussian.set_covar(clip_covar(gaussian.covar(),min_covar));
  }
}

#endif // bsta_fit_gaussian_h_
