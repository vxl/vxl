#include <testlib/testlib_test.h>
#include <bsta/bsta_mixture.h>
#include <vcl_string.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_basic_functors.h>

template <class T>
void test_mixture_type(T dummy, const vcl_string& type_name)
{
  // test stuff here
  TEST(("Dummy test <"+type_name+">").c_str(), true, true);

}
void test_probability_functor()
{
  vcl_cout << "Starting test of probability functors\n";
  const float covar = 0.01f;
  const float mod_covar = 0.02f;
  const float sample = 0.25f;
  typedef bsta_gauss_f1 bsta_gauss_t;
  typedef bsta_gauss_t::covar_type covar_t;
  typedef bsta_gauss_t::vector_type vector_t;
  typedef bsta_mixture<bsta_gauss_t> mix_gauss_type;
  bsta_gauss_t gauss0(vector_t(0.0f), covar_t(covar) );  
  bsta_gauss_t gauss1(vector_t(0.5f), covar_t(covar) );  
  mix_gauss_type mix;
  bool good = mix.insert(gauss0, 0.8f);
  good = good&& mix.insert(gauss1, 0.2f);
  bsta_prob_density_addcovar_functor<mix_gauss_type> pd;
  float probd;
  good = good && pd(mix, sample, mod_covar, probd);
  TEST("test mixture", good, true);
  TEST_NEAR("probability density", probd, 0.81275439, 1e-06);
  // test the probability functor
  bsta_probability_addcovar_functor<mix_gauss_type> prb;
  float prob;
  float minp = sample - 0.1f, maxp = sample + 0.1f;
  good = prb(mix, minp, maxp, mod_covar, prob);
  TEST_NEAR("probability", prob,0.17158404398 , 1e-06);
  // make sure the covariance is restored
  float v0 = mix.distribution(0).covar();
  float v1 = mix.distribution(1).covar();
  TEST_NEAR("restored covariance", v0+v1, 0.02, 1e-06);
  
}
MAIN( test_mixture )
{
  START ("mixture");
#if 0
  test_mixture_type(float(),"float");
  test_mixture_type(double(),"double");
#endif
  test_probability_functor();
  SUMMARY();
}

