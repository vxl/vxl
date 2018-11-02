#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_basic_functors.h>

template <class T>
void test_mixture_type(T /*dummy*/, const std::string& type_name)
{
  // test stuff here
  TEST(("Dummy test <"+type_name+">").c_str(), true, true);
}

void test_probability_functor()
{
  std::cout << "Starting test of probability functors\n";
  const float covar = 0.01f;
  const float mod_covar = 0.02f;
  const float sample = 0.25f;
  typedef bsta_gauss_sf1 bsta_gauss_t;
  typedef bsta_gauss_t::covar_type covar_t;
  typedef bsta_gauss_t::vector_type vector_t;
  typedef bsta_mixture<bsta_gauss_t> mix_gauss_type;
  bsta_gauss_t gauss0(vector_t(0.0f), covar_t(covar) );
  bsta_gauss_t gauss1(vector_t(0.5f), covar_t(covar) );
  mix_gauss_type mix;
  bool good1 = mix.insert(gauss0, 0.8f);
  bool good2 = mix.insert(gauss1, 0.2f);
  bsta_prob_density_addcovar_functor<mix_gauss_type> pd;
  float probd;
  bool good3 = pd(mix, sample, mod_covar, probd); // returns probd
  TEST("test mixture", good1 && good2 && good3, true);
  TEST_NEAR("probability density", probd, 0.81275439, 1e-6);
  // test the probability functor
  bsta_probability_addcovar_functor<mix_gauss_type> prb;
  float prob;
  float minp = sample - 0.1f, maxp = sample + 0.1f;
  prb(mix, minp, maxp, mod_covar, prob);
  TEST_NEAR("probability", prob, 0.17158404398, 1e-6);
  // make sure the covariance is restored
  float v0 = mix.distribution(0).covar();
  float v1 = mix.distribution(1).covar();
  TEST_NEAR("restored covariance", v0+v1, 0.02, 1e-6);
}

void test_sampling()
{
  std::cout << "Starting test of sampling\n";
  typedef bsta_gauss_sf1 bsta_gauss_t;
  typedef bsta_gauss_t::covar_type covar_t;
  typedef bsta_gauss_t::vector_type vector_t;
  const float covar = 0.01f;
  bsta_gauss_t gauss0(vector_t(0.0f), covar_t(covar) );
  bsta_gauss_t gauss1(vector_t(0.5f), covar_t(covar) );
  float mean = 0.0f;
  vnl_random rng;
  for (unsigned i = 0; i < 1000; i++) {
    vector_t s0 = gauss0.sample(rng);
    std::cout << s0 << ' ';
    mean += s0;
  }
  std::cout << std::endl;

  mean = 0.0f;
  for (unsigned i = 0; i < 1000; i++) {
    vector_t s1 = gauss1.sample(rng);
    std::cout << s1 << ' ';
    mean += s1;
  }
  std::cout << std::endl;

#if 0
  typedef bsta_mixture<bsta_gauss_t> mix_gauss_type;
  mix_gauss_type mix;
  bool good = mix.insert(gauss0, 0.8f);
  good = good && mix.insert(gauss1, 0.2f);
  bsta_prob_density_addcovar_functor<mix_gauss_type> pd;
  float probd;
  const float sample = 0.25f;
  const float mod_covar = 0.02f;
  good = good && pd(mix, sample, mod_covar, probd);
  TEST("test mixture", good, true);
  TEST_NEAR("probability density", probd, 0.81275439, 1e-6);
  // test the probability functor
  bsta_probability_addcovar_functor<mix_gauss_type> prb;
  float prob;
  float minp = sample - 0.1f, maxp = sample + 0.1f;
  prb(mix, minp, maxp, mod_covar, prob);
  TEST_NEAR("probability", prob, 0.17158404398, 1e-6);
  // make sure the covariance is restored
  float v0 = mix.distribution(0).covar();
  float v1 = mix.distribution(1).covar();
  TEST_NEAR("restored covariance", v0+v1, 0.02, 1e-6);
#endif // 0
}

static void test_mixture()
{
  test_mixture_type(float(),"float");
  test_mixture_type(double(),"double");
  test_probability_functor();
  test_sampling();
}

TESTMAIN(test_mixture);
