// This is mul/mbl/tests/test_histogram.cxx
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <mbl/mbl_histogram.h>
#include <testlib/testlib_test.h>

void test_histogram()
{
  vcl_cout << "***********************\n"
           << " Testing mbl_histogram\n"
           << "***********************\n";

  mbl_histogram histo;

  TEST("Empty N.obs",histo.n_obs()==0,true);

  histo.set_bins(1,4,3);
  TEST("n_bins()=3",histo.n_bins()==3,true);
  TEST("Lowest bin",vcl_fabs(histo.bins()[0]-1)<1e-6,true);
  TEST("Highest bin",vcl_fabs(histo.bins()[3]-4)<1e-6,true);

  for (int i=0;i<10;++i)
  {
    double v = 0.1+i*0.5;
    histo.obs(v);
  }

  TEST("n_below()",histo.n_below()==2,true);
  TEST("n_above()",histo.n_above()==2,true);
  TEST("n_obs()",histo.n_obs()==10,true);
  TEST("bin 1",histo.frequency()[0]==2,true);

  vcl_cout<<histo<<vcl_endl;

  mbl_histogram histo2 = histo;
  TEST("Equality operator",histo==histo2,true);
}

TESTMAIN(test_histogram);
