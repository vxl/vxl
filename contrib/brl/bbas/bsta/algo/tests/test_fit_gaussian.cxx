#include <iostream>
#include <string>
#include <testlib/testlib_test.h>

#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/bsta_gaussian_sphere.h>
#include <bsta/algo/bsta_fit_gaussian.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_random.h>


MAIN( test_fit_gaussian)
{
  START ("test_fit_gaussian");
  // seed the random number generator for better test repeatability
  vnl_random rand_gen(468002);

  {
    std::cout << "Testing 1D Gaussian (spherical)" << std::endl;
    // Test the fitting of spherical gaussian
    // 1D
    float mean = 2.3f;
    float var = 0.2f;
    float sigma = std::sqrt(var);

    constexpr unsigned int n_samples = 500;

    bsta_gaussian_sphere<float,1> gauss_est;

    // generate samples
    std::vector<float> samples;
    std::vector<float> weights;
    for (unsigned int n=0; n<n_samples; ++n) {
      float sample = mean + sigma*(float)rand_gen.normal();
      auto weight = (float)rand_gen.drand32(0.0, 1.0);
      weights.push_back(weight);
      samples.push_back(sample);
    }
    bsta_fit_gaussian(samples, weights, gauss_est);

    TEST_NEAR("1D: Estimated mean is correct" ,gauss_est.mean(), mean, 0.1);
    TEST_NEAR("1D: Estimated variance is correct", gauss_est.var(), var, 0.05);
  }
  {
    std::cout << "Testing 3D Gaussian (spherical)" << std::endl;
    // Test the fitting of spherical gaussian
    // 3D
    vnl_vector_fixed<float,3> mean(2.5f, 1.2f, -0.85f);
    float var = 0.25f;

    float sigma = std::sqrt(var);

    constexpr unsigned int n_samples = 500;

    bsta_gaussian_sphere<float,3> gauss_est;

    // generate samples
    std::vector<vnl_vector_fixed<float,3> > samples;
    std::vector<float> weights;
    for (unsigned int n=0; n<n_samples; ++n) {
      vnl_vector_fixed<float,3> sample;
      for (unsigned int d=0; d<3; ++d) {
        sample[d] = mean[d] + sigma*(float)rand_gen.normal();
      }
      auto weight = (float)rand_gen.drand32(0.0, 1.0);
      weights.push_back(weight);
      samples.push_back(sample);
    }
    bsta_fit_gaussian(samples, weights, gauss_est);
    std::cout << "mean = " << mean << '\n'
             << "estimated mean = " << gauss_est.mean() << '\n'
             << "variance = " << var << '\n'
             << "estimated variance = " << gauss_est.covar() << std::endl;
    TEST_NEAR("3D spherical: Estimated mean maximum error is small" ,(gauss_est.mean() - mean).max_value(), 0, 0.1);

    TEST_NEAR("3D spherical: Estimated variance is correct", gauss_est.covar(), var, 0.1);
  }
  {
    std::cout << "Testing 3D Gaussian (independent)" << std::endl;
    // Test the fitting of independent gaussian
    vnl_vector_fixed<float,3> mean(2.5f, 1.2f, -0.85f);
    vnl_vector_fixed<float,3> covar_diag(0.3f, 0.09f, 0.55f);

    vnl_vector_fixed<float,3> sigmas(std::sqrt(covar_diag[0]),std::sqrt(covar_diag[1]),std::sqrt(covar_diag[2]));

    constexpr unsigned int n_samples = 1000;

    bsta_gaussian_indep<float,3> gauss_est;
    // generate samples
    std::vector<vnl_vector_fixed<float,3> > samples;
    std::vector<float> weights;
    for (unsigned int n=0; n<n_samples; ++n) {
      vnl_vector_fixed<float,3> sample;
      for (unsigned int d=0; d<3; ++d) {
        sample[d] = mean[d] + sigmas[d]*(float)rand_gen.normal();
      }
      auto weight = (float)rand_gen.drand32(0.0, 1.0);
      weights.push_back(weight);
      samples.push_back(sample);
    }
    bsta_fit_gaussian(samples, weights, gauss_est);

    std::cout << "mean = " << mean << '\n'
             << "estimated mean = " << gauss_est.mean() << '\n'
             << "variance = " << covar_diag << '\n'
             << "estimated variance = " << gauss_est.covar() << std::endl;
    TEST_NEAR("3D indep: Estimated mean maximum error is small" ,(gauss_est.mean() - mean).max_value(), 0, 0.1);
    TEST_NEAR("3D indep: Estimated covariance maximum error is small" ,(gauss_est.covar() - covar_diag).max_value(), 0, 0.15);
  }
  {
    std::cout << "Testing 3D Gaussian (full covariance matrix)" << std::endl;
    // Test the fitting of independent gaussian
    vnl_vector_fixed<float,3> mean(5.5f, -2.2f, 0.25f);
    vnl_matrix_fixed<float,3,3> A(0.0f); // lower triangular matrix used to construct covariance
    A(0,0) = 1.5f; A(1,0) = 0.05f; A(1,1) = 1.1f; A(2,0) = 0.08f; A(2,1) = -0.10f; A(2,2) = 0.7f;
    vnl_matrix_fixed<float,3,3> covar = A * A.transpose();

    constexpr unsigned int n_samples = 4000;

    bsta_gaussian_full<float,3> gauss_est;
    // generate samples
    std::vector<vnl_vector_fixed<float,3> > samples;
    std::vector<float> weights;
    for (unsigned int n=0; n<n_samples; ++n) {
      vnl_vector_fixed<float,3> sample;
      vnl_vector_fixed<float,3> unit_rand;
      for (unsigned int d=0; d<3; ++d) {
        unit_rand[d] = (float)rand_gen.normal();
      }
      sample = mean + A * unit_rand;
      auto weight = (float)rand_gen.drand32(0.0, 1.0);
      weights.push_back(weight);
      samples.push_back(sample);
    }
    bsta_fit_gaussian(samples, weights, gauss_est);
    vnl_matrix_fixed<float,3,3> covar_est = gauss_est.covar();

    std::cout << "mean = " << mean << '\n'
             << "estimated mean = " << gauss_est.mean() << '\n'
             << "covar =\n" << covar << '\n'
             << "estimated covar =\n" << covar_est << std::endl;

    TEST_NEAR("3D full: Estimated mean maximum error is small" ,(gauss_est.mean() - mean).max_value(), 0, 0.1);

    float total_error = (covar - covar_est).absolute_value_max();
    TEST_NEAR("  Estimated covariance matrix is error is small", total_error, 0.0f, 0.25);
  }
  {
    // test EM fitting.
    // TODO
    bsta_gaussian_sphere<float,1> gauss_est;
    std::vector<float> samples;
    std::vector<float> sample_prob;
    std::vector<float> alt_prob_density;
    float min_var = 0.01f;
    bsta_fit_gaussian(samples,sample_prob,alt_prob_density,gauss_est,min_var);
  }
  {
    // TODO
    bsta_gaussian_indep<float,3> gauss_est;
    std::vector<vnl_vector_fixed<float,3> > samples;
    std::vector<float> sample_prob;
    std::vector<float> alt_prob_density;
    vnl_vector_fixed<float,3> min_covar(0.01f);
    bsta_fit_gaussian(samples,sample_prob,alt_prob_density,gauss_est, min_covar);
  }
  {
      // TODO
    bsta_gaussian_full<float,3> gauss_est;
    std::vector<vnl_vector_fixed<float,3> > samples;
    std::vector<float> sample_prob;
    std::vector<float> alt_prob_density;
    vnl_matrix_fixed<float,3,3> min_covar(0.0f);
    min_covar(0,0) = 0.01f; min_covar(1,1) = 0.01f; min_covar(2,2) = 0.01f;
    bsta_fit_gaussian(samples,sample_prob,alt_prob_density,gauss_est, min_covar);
  }

  SUMMARY();
}
