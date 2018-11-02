#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brec/brec_param_estimation.h>

static void test_param_estimation()
{
  std::vector<std::pair<float, float> > pairs;
  pairs.push_back(std::pair<float, float>(0.2f, 0.4f));
  pairs.push_back(std::pair<float, float>(0.3f, 0.4f));
  pairs.push_back(std::pair<float, float>(0.4f, 0.4f));
  pairs.push_back(std::pair<float, float>(0.4f, 0.5f));
  pairs.push_back(std::pair<float, float>(0.6f, 0.6f));
  pairs.push_back(std::pair<float, float>(0.8f, 0.7f));
  pairs.push_back(std::pair<float, float>(0.8f, 0.7f));
  pairs.push_back(std::pair<float, float>(0.8f, 0.5f));
  pairs.push_back(std::pair<float, float>(0.5f, 0.5f));
  pairs.push_back(std::pair<float, float>(0.5f, 0.6f));
  pairs.push_back(std::pair<float, float>(0.5f, 0.7f));
  pairs.push_back(std::pair<float, float>(0.5f, 0.8f));
  pairs.push_back(std::pair<float, float>(0.5f, 0.9f));
  pairs.push_back(std::pair<float, float>(0.5f, 0.9f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.9f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.8f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.8f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.6f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.3f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.1f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.2f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.8f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.3f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.1f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.1f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.5f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.1f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.1f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.2f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.1f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.2f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.2f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.2f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.2f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.1f));
  pairs.push_back(std::pair<float, float>(0.1f, 0.1f));

  double init_sigma = brec_param_estimation::estimate_fg_pair_density_initial_sigma(pairs);
  std::cout << "initial sigma: " << init_sigma << std::endl;

  //TEST_NEAR("testing param estimation ", brec_param_estimation::estimate_fg_pair_density_sigma(pairs, init_sigma), 0.22, 0.001);
  TEST_NEAR("testing param estimation ", brec_param_estimation::estimate_fg_pair_density_sigma_amoeba(pairs, init_sigma), 0.22, 0.1);
}

TESTMAIN( test_param_estimation );
