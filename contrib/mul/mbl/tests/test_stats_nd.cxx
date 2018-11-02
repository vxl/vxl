// This is mul/mbl/tests/test_stats_nd.cxx
#include <iostream>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_stats_nd.h>
#include <testlib/testlib_test.h>

void test_stats_nd()
{
  std::cout << "**********************\n"
           << " Testing mbl_stats_nd\n"
           << "**********************\n";

  mbl_stats_nd stats;

  TEST("Empty N.obs",stats.n_obs(), 0);
  TEST("Empty mean",stats.mean().size(), 0);

  vnl_vector<double> x(4);
  for (int i=0;i<5;++i)
  {
    x.fill(i);
    stats.obs(x);
  }

  TEST("nObs()",stats.n_obs(), 5);

  x.fill(2.0);
  TEST_NEAR("mean()",vnl_vector_ssd(stats.mean(),x), 0.0, 1e-6);
  x.fill(10.0);
  TEST_NEAR("sum()",vnl_vector_ssd(stats.sum(),x), 0.0, 1e-6);
  x.fill(30.0);
  TEST_NEAR("sumSq()",vnl_vector_ssd(stats.sumSq(),x), 0.0, 1e-6);
  x.fill(2.5);
  TEST_NEAR("variance()",vnl_vector_ssd(stats.variance(),x), 0.0, 1e-6);
  x.fill(std::sqrt(2.5));
  TEST_NEAR("sd()",vnl_vector_ssd(stats.sd(),x), 0.0, 1e-6);
  x.fill(std::sqrt(0.5));
  TEST_NEAR("stdError()",vnl_vector_ssd(stats.stdError(),x), 0.0, 1e-6);

  std::cout<<stats<<std::endl;

  mbl_stats_nd stats2 = stats;
  TEST("Equality operator",stats, stats2);
}

TESTMAIN(test_stats_nd);
