// This is mul/mbl/tests/test_stats_nd.cxx
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <mbl/mbl_stats_nd.h>
#include <testlib/testlib_test.h>

void test_stats_nd()
{
  vcl_cout << "**********************\n"
           << " Testing mbl_stats_nd\n"
           << "**********************\n";

  mbl_stats_nd stats;

  TEST("Empty N.obs",stats.n_obs()==0,true);
  TEST("Empty mean",stats.mean().size()==0,true);

  vnl_vector<double> x(4);
  for (int i=0;i<5;++i)
  {
    x.fill(i);
    stats.obs(x);
  }

  TEST("nObs()",stats.n_obs()==5,true);

  x.fill(2.0);
  TEST("mean()",vnl_vector_ssd(stats.mean(),x)<1e-6,true);
  x.fill(10.0);
  TEST("sum()",vnl_vector_ssd(stats.sum(),x)<1e-6,true);
  x.fill(30.0);
  TEST("sumSq()",vnl_vector_ssd(stats.sumSq(),x)<1e-6,true);
  x.fill(2.5);
  TEST("variance()",vnl_vector_ssd(stats.variance(),x)<1e-6,true);
  x.fill(vcl_sqrt(2.5));
  TEST("sd()",vnl_vector_ssd(stats.sd(),x)<1e-6,true);
  x.fill(vcl_sqrt(0.5));
  TEST("stdError()",vnl_vector_ssd(stats.stdError(),x)<1e-6,true);

  vcl_cout<<stats<<vcl_endl;

  mbl_stats_nd stats2 = stats;
  TEST("Equality operator",stats==stats2,true);
}

TESTMAIN(test_stats_nd);
