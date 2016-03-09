
#include <testlib/testlib_test.h>
#include <vnl/vnl_random.h>
#include <boxm/sample/algo/boxm_sigma_normalizer.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/algo/bsta_fit_gaussian.h>


static void test_sigma_normalizer()
{
  START("boxm_sigma_normalizer test");

  // generate random sets of samples from a normal distribution and compare sigma estimate to true value
  {
    vnl_random rand_gen;
    unsigned int n_trials = 5000;

    unsigned int n_observations = 6;
    float true_sigma = 0.5f;
    float true_mean = 2.5f;
    float under_estimation_prob = 0.4f;
    unsigned int under_estimates = 0;

    boxm_sigma_normalizer normalizer(under_estimation_prob);

    for (unsigned int t=0; t<n_trials; ++t) {
      std::vector<float> samples;
      std::vector<float> weights;
      for (unsigned int n=0; n<n_observations; ++n) {
        float sample = true_mean + true_sigma*(float)rand_gen.normal();
        samples.push_back(sample);
        weights.push_back(1.0f);
      }
      // compute the sample mean and variance
      bsta_gauss_sf1 gauss_model;
      bsta_fit_gaussian(samples, weights, gauss_model);
      float normalized_sigma = std::sqrt(gauss_model.var()) * normalizer.normalization_factor_int(n_observations);
      if (normalized_sigma < true_sigma) {
        ++under_estimates;
      }
    }
    float under_estimate_ratio = (float)under_estimates / n_trials;

    TEST_NEAR("correct ratio of sigma under-estimates", under_estimate_ratio, under_estimation_prob, 0.05);
  }

  // make sure normalization factor approaches 1 for large sample sizes
  {
    boxm_sigma_normalizer sigma_norm(0.10f);
    float norm_factor_f = sigma_norm.normalization_factor(10000.0f);
    float norm_factor_i = sigma_norm.normalization_factor_int(10000);

    TEST_NEAR("normalization factor (float) approaches 1 for large N",norm_factor_f, 1.0, 0.1);
    TEST_NEAR("normalization factor (int) approaches 1 for large N",norm_factor_i, 1.0, 0.1);
  }
  return;
}

TESTMAIN(test_sigma_normalizer);
