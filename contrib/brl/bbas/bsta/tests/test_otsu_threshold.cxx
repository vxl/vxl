//:
// \file
#include <testlib/testlib_test.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_otsu_threshold.h>
#include <vcl_iostream.h>

//: Test bsta otsu_thresholds
void test_otsu_threshold()
{
  double range = 128.0;
  int bins = 16;
  bsta_histogram<double> h(range, bins);
  h.upcount(0, 0);
  h.upcount(8, 10);
  h.upcount(16, 50);
  h.upcount(24, 100);
  h.upcount(32, 100);
  h.upcount(40, 50);
  h.upcount(48, 10);
  h.upcount(56, 0);
  h.upcount(64, 10);
  h.upcount(72, 50);
  h.upcount(80, 100);
  h.upcount(88, 100);
  h.upcount(96, 50);
  h.upcount(104, 10);
  h.upcount(112, 0);
  
  vcl_cout << "Counts\n";
  for(unsigned i = 0; i<static_cast<unsigned>(bins); ++i)
    vcl_cout << "c[" << i << "] = " << h.counts(i) << '\n';
  double mean  = h.mean(2, 7);
  double variance  = h.variance(2, 7);
  vcl_cout << " Mean = " << mean << "  Variance = " << variance << '\n';

  TEST_NEAR("mean between bin indices"          , mean,           24.2963, 1e-4);
  TEST_NEAR("variance between bin indices", variance, 82.8752, 1e-4);
  

  bsta_otsu_threshold<double> ot(h);
  unsigned t = ot.bin_threshold();
  vcl_cout << "Otsu Threshold Bin Index " << t << '\n';
  TEST("otsu_bin_threshold", t, 6);
  double dt = ot.threshold();
  vcl_cout << "Otsu Threshold " << dt << '\n';
  TEST_NEAR("otsu_threshold"   , dt,  52 ,1e-06);
  
}

TESTMAIN(test_otsu_threshold);
