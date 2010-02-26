//:
// \file
// \brief A class for adjusting sample standard deviation values such that the probability of underestimation of the true std. dev. is fixed.

#include <vcl_iostream.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_gamma.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include "boxm_sigma_normalizer.h"

boxm_sigma_normalizer::boxm_sigma_normalizer(float under_estimation_probability) : unbias_const_(N_PRECOMPUTED_+1, 0.0f)
{
  class gammainc_error_fn : public vnl_least_squares_function
  {
   public:
    gammainc_error_fn(unsigned int ndof, float under_estimation_prob) : vnl_least_squares_function(1,1,no_gradient), ndof_(ndof), p_(under_estimation_prob) {}

    virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx)
    {
      // don't allow x to become negative
      if (x[0] < 0.0f) {
        fx[0] = x[0] - p_; // so error function is c0 continuous at x=0
        return;
      }
      double cdf_prob = vnl_cum_prob_chi2(ndof_, x[0]);
      fx[0] = cdf_prob - p_;
      return;
    }
   private:
    unsigned int ndof_;
    float p_;
  };

  // sanity check on probability
  if (under_estimation_probability < 1e-4) {
    vcl_cout << "error : boxm_sigma_normalizer : under_estimation_probability " << under_estimation_probability << " too low " << vcl_endl;
    return;
  }
  if (under_estimation_probability > (1 - 1e-4)) {
    vcl_cout << "error : boxm_sigma_normalizer : under_estimation_probability " << under_estimation_probability << " too high " << vcl_endl;
    return;
  }

  // populate array of normalization factors for easy lookup
  // sigma is undefined for sample sizes of 0 and 1, just fill with large values.
  unbias_const_[0] = 1e3f;
  unbias_const_[1] = 1e3f;

  for (unsigned int n=2; n<= N_PRECOMPUTED_; ++n) {
    gammainc_error_fn f(n-1, under_estimation_probability);
    vnl_levenberg_marquardt minimizer(f);

    vnl_vector_fixed<double,1> x(1.0f);
    minimizer.minimize(x);

    float unbias_constant = (float)vcl_sqrt((float)(n-1) / x[0]);

    unbias_const_[n] = unbias_constant;
  }
}


float boxm_sigma_normalizer::normalization_factor(float number_of_observations)
{
  if (number_of_observations <= 1.0f) {
    return normalization_factor_int((unsigned int)1);
  }

  // linearly interpolate between integer values
  float nobs_floor = vcl_floor(number_of_observations);
  float nobs_ceil = vcl_ceil(number_of_observations);
  float floor_weight = nobs_ceil - number_of_observations;
  float norm_factor = (normalization_factor_int((unsigned int)nobs_floor) * floor_weight) + (normalization_factor_int((unsigned int)nobs_ceil) * (1.0f - floor_weight));

  return norm_factor;
}


float boxm_sigma_normalizer::normalization_factor_int(unsigned int number_of_observations)
{
  if (number_of_observations < 2) {
    return unbias_const_[1];
  }

  if (number_of_observations <= N_PRECOMPUTED_) {
    return unbias_const_[number_of_observations];
  }
  // else nobs >= N_PRECOMPUTED_
  // approximate for big n with function a = m /nobs + b
  static const float m = (unbias_const_[N_PRECOMPUTED_] - unbias_const_[N_PRECOMPUTED_ - 5])/(1.0f/N_PRECOMPUTED_ - 1.0f/(N_PRECOMPUTED_ - 5));
  static const float b = unbias_const_[N_PRECOMPUTED_]  - m*(1.0f/N_PRECOMPUTED_);
  return m/number_of_observations + b;
}


