// This is mul/mbl/tests/test_stats_1d.cxx
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <mbl/mbl_stats_1d.h>
#include <testlib/testlib_test.h>

void test_stats_1d()
{
  vcl_cout << "**********************\n"
           << " Testing mbl_stats_1d\n"
           << "**********************\n";

  mbl_stats_1d stats;

  TEST("Empty N.obs",stats.nObs()==0,true);
  TEST("Empty mean",stats.mean()==0.0,true);

  for (int i=0;i<5;++i)
    stats.obs(i);

  TEST("nObs()",stats.nObs()==5,true);
  TEST("mean()",vcl_fabs(stats.mean()-2)<1e-6,true);
  TEST("sum()",vcl_fabs(stats.sum()-10)<1e-6,true);
  TEST("sumSq()",vcl_fabs(stats.sumSq()-30)<1e-6,true);

  vcl_cout<<stats<<vcl_endl;

  mbl_stats_1d stats2 = stats;
  TEST("Equality operator",stats==stats2,true);
}

TESTMAIN(test_stats_1d);
