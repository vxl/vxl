#include <iostream>
#include <string>
#include <testlib/testlib_test.h>

#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/algo/bsta_adaptive_updater.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// This test was first written to verify whether the basic functionality of
// bsta_mixtures is consistent across platforms.
// It is only tested for gaussian 3-modal, floating point mixtures in 2, 3,
// and 4 dimensions. More cases can be added when needed.

// Test the update of independent Gaussian distributions
template <class T, unsigned n>
void test_gauss_indep_update()
{
  //A tri-mixture of n-dimensional independent gaussian
  typedef bsta_num_obs<bsta_gaussian_indep<T,n> > gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;


  //Initialize the updater
  float init_variance = 0.008f;
  float min_stddev = 0.02f;
  float g_thresh = 2.5;
  typename bsta_gaussian_indep<T,n>::covar_type init_covar(init_variance);
  bsta_gaussian_indep<T,n> init_gauss(typename gauss_type::vector_type(0.0f),init_covar);

  bsta_mg_grimson_weighted_updater<mix_gauss> updater(init_gauss,n,g_thresh,min_stddev);

  //Update

  //Create the model, the observation and the weight
  mix_gauss_type model;
  typename gauss_type::vector_type obs(0.3f);
  float weight = 0.01f;

  updater(model, obs, weight);
  obs = typename gauss_type::vector_type(0.8f);
  updater(model, obs, weight);
  obs = typename gauss_type::vector_type(0.81f);
  updater(model, obs, weight);

  //Test most probable value:
 typename gauss_type::vector_type most_probable_val(T(0));
  if (model.num_components() > 0)
    if (model.weight(0) > 0.0f)
      most_probable_val = model.distribution(0).mean();

  for (unsigned i = 0 ; i<n; i++)
  {
    T mp_v = most_probable_val[i];
    TEST_NEAR("most_probable", mp_v , T(0.8), T(0.01));
  }

  //Test expected value:
  typename gauss_type::vector_type v(T(0));
  typename gauss_type::vector_type expected_val(T(0));

  expected_val = model.expected_value();

  for (unsigned i = 0 ; i<n; i++)
  {
    T e_v = expected_val[i];
    TEST_NEAR("expected_value", e_v , T(0.63), T(0.01));
  }
}


static void test_gaussian_model()
{
  std::cout << "----------------------------------\n"
           << " float, 2-dimensional, 3-modal\n"
           << "-----------------------------------" <<std::endl;
  test_gauss_indep_update<float,2>();
  std::cout << "-----------------------------------\n"
           << " float, 3-dimensional, 3-modal\n"
           << "-----------------------------------" <<std::endl;
  test_gauss_indep_update<float,3>();
  std::cout << "-----------------------------------\n"
           << " float, 4-dimensional, 3-modal\n"
           << "-----------------------------------" <<std::endl;
  test_gauss_indep_update<float,4>();
}

TESTMAIN(test_gaussian_model);
