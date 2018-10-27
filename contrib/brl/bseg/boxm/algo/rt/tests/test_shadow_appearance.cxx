#include <testlib/testlib_test.h>

#include <boxm/boxm_apm_traits.h>
#include <boxm/algo/rt/boxm_shadow_appearance_estimator.h>
#include <boct/boct_tree.h>
#include <boxm/algo/rt/boxm_shadow_app_initializer.h>

namespace test_shadow_appearance_globals
{
  typedef boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype obs_type;
  typedef boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype app_type;
}


static void test_shadow_appearance()
{
  using namespace test_shadow_appearance_globals;
  //normalized observations square building (I-A)/(mu+ - A), partly in shadow
  double obs_arr[] = {0.600555,0.7,0.061111,0.748335,0.77611,0.68611,
                      0.148889,0.855555,0.664445,0.651665,0.101111,
                      0.0272222,0.67278,0.70389,0.1733335,0.77111,
                      0.701665,0.64778,0.65111,0.72611,0.82889,0.735,
                      0.077222,0.0816665,0.051111,0.71722,0.61722,
                      0.065,0.095,0.0866665,0.58611,0.665555};
  std::vector<obs_type> obs;
  std::vector<float> pre, vis;
  for (double i : obs_arr) {
    obs.push_back(static_cast<obs_type>(i));
    pre.push_back(0.0f);
    vis.push_back(1.0f);
  }
  float min_app_sigma = 0.05f;
  float shadow_prior = 0.5f;
  float shadow_mean = 0.0f;
  float shadow_sigma = 0.05f;
  bool verbose = true;
  app_type model;
  boxm_compute_shadow_appearance<BOXM_APM_SIMPLE_GREY>(obs, pre, vis,
                                                       model,
                                                       min_app_sigma,
                                                       shadow_prior,
                                                       shadow_mean,
                                                       shadow_sigma,
                                                       verbose);
  double er = std::fabs(0.70034355-model.color())+
    std::fabs(model.sigma()- 1.0/11.954589);
  TEST_NEAR("test shadow appearance EM", er, 0.0, 1e-6);
  // normalized observations for dark road surface
double dark_obs_arr[] ={0.0477778,0.0666665,0.0072222,0.175,0.12,
                        0.162778,0.003333335,0.188889,0.233889,
                        0.0605555,0.0077778,0.004444445,0.0883335,
                        0.15,0.0716665,0.1233335,0.1983335,0.15,
                        0.0883335,0.03944445,0.122222,0.125,0.25,
                        0.005,0.0122222,0.1,0.1166665,0.03166665,
                        0.015,0.01,0.1016665,0.167222};
  std::vector<obs_type> dark_obs;
  for (double i : dark_obs_arr)
    dark_obs.push_back(static_cast<obs_type>(i));
  app_type dark_model;
  boxm_compute_shadow_appearance<BOXM_APM_SIMPLE_GREY>(dark_obs, pre, vis,
                                                       dark_model,
                                                       min_app_sigma,
                                                       shadow_prior,
                                                       shadow_mean,
                                                       shadow_sigma,
                                                       verbose);
  double dark_er = std::fabs(0.14765342-dark_model.color())
                 + std::fabs(dark_model.sigma()- 1.0/16.097824);
  TEST_NEAR("test dark surface shadow appearance EM", dark_er, 0.0, 1e-6);
}

TESTMAIN(test_shadow_appearance);
