// This is mul/mbl/tests/test_sum_1d.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_sum_1d.h>
#include <testlib/testlib_test.h>

void test_sum_1d()
{
  std::cout << "********************\n"
           << " Testing mbl_sum_1d\n"
           << "********************\n";

  mbl_sum_1d stats,stats2;

  TEST("Empty N.obs",stats.nObs(), 0);
  TEST("Empty mean",stats.mean(), 0.0);

  for (int i=0;i<5;++i)
  {
    stats.obs(i);
    stats2.obs(i+5);
  }

  TEST("nObs()",stats.nObs(), 5);
  TEST_NEAR("mean()",stats.mean(), 2, 1e-6);

  std::cout<<stats<<std::endl;

  mbl_sum_1d stats3 = stats;
  TEST("Equality operator",stats, stats3);

  mbl_sum_1d stats4 = stats+stats2;
  TEST("Addition nObs()",stats4.nObs(), 10);
  TEST_NEAR("Addition mean()",stats4.mean(), 4.5, 1e-6);
}

TESTMAIN(test_sum_1d);
