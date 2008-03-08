//:
// \file
#include <testlib/testlib_test.h>
#include <vcl_cmath.h>
#include <vcl_typeinfo.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_joint_histogram.h>
#include <bsta/bsta_int_histogram_1d.h>
#include <bsta/bsta_int_histogram_2d.h>
#include <vpl/vpl.h>
#include <vcl_iostream.h>
#include <bsta/io/bsta_io_histogram.h>
#include <vsl/vsl_binary_io.h>
#include <bsta/bsta_histogram_sptr.h>
#include <bsta/bsta_joint_histogram_sptr.h>
static void test_int_hist()
{
  bsta_int_histogram_1d h1d(50);
  vcl_cout << "Num 1d bins "  << h1d.get_nbins() << '\n';
  TEST("Test No Bins 1d", h1d.get_nbins(), 50);
  for(unsigned k = 0; k<50; ++k)
    h1d.set_count(k, k*k);
  unsigned int index;
  unsigned long int max_value = h1d.get_max_val(index);
  vcl_cout << "Max value " << max_value << " at index " << index << '\n';
  
  bsta_int_histogram_2d h2d(50, 50);

  unsigned int imax_x = 0; 
  unsigned int imax_y = 0;
  h2d.get_max_val(imax_x, imax_y);

}

//: Test bsta histograms
void test_bsta_histogram()
{
  double range = 128.0;
  int bins = 16;
  double delta = range/bins;
  bsta_histogram<double> h(range, bins);
  double v = 0.0;
  for (int b =0; b<bins; b++, v+=delta)
    h.upcount(v, 1.0);
  vcl_cout << "Bins\n";
  h.print();
  double area = h.area();
  double fraction_below = h.fraction_below(33.0);
  double fraction_above = h.fraction_above(96.0);
  double value_below = h.value_with_area_below(0.25);
  double value_above = h.value_with_area_above(0.25);

  TEST_NEAR("area"          , area,           16.0, 1e-9);
  TEST_NEAR("fraction_below", fraction_below, 5/16.0, 1e-9);
  TEST_NEAR("fraction_above", fraction_above, 3/16.0, 1e-9);
  TEST_NEAR("value_below"   , value_below ,   56.0-32, 1e-9);
  TEST_NEAR("value_above"   , value_above ,   56.0+32, 1e-9);

  //Test data constructor
  vcl_vector<double> data(16, 1.0);
  bsta_histogram<double> hdata(0, 128, data);
  hdata.upcount(32,1);
  vcl_cout << "Bins\n";
  hdata.print();
  vcl_cout << "p(32) " << hdata.p(32.0) << '\n';
  TEST_NEAR("test data constructor p(32.0)", hdata.p(32.0), 0.117647, 1e-6);

  //Test entropy
  double ent = h.entropy();
  vcl_cout << "Uniform Entropy for " << bins << " bins = " << ent  << " bits.\n";
  TEST_NEAR("test histogram uniform distribution entropy", ent, 31.0/8, 1e-9);

  //Joint Histogram Tests
  bsta_joint_histogram<double> jh(range, bins);
  double va = 0;
  for (int a =0; a<bins; a++, va+=delta)
  {
    double vb = 0;
    for (int b =0; b<bins; b++, vb+=delta)
      jh.upcount(va, 1.0, vb, 1.0);
  }
  double jent = jh.entropy();
  vcl_cout << "Uniform Joint Entropy for " << bins*bins << " bins = " << jent  << " bits.\n";

  TEST_NEAR("test joint histogram uniform distribution entropy", jent, 31.0/4, 1e-9);
  test_int_hist();
 //=================================================
  // test binary io for histogram classes
  // 1-d histogram
  double nbinsd = h.nbins();
  double cn = h.counts(3);
  double max = h.max();
  vsl_b_ofstream os("./temp.bin");
  vsl_b_write(os, h);
  os.close();
  vsl_b_ifstream is("./temp.bin");
  bsta_histogram<double> h_in;
  vsl_b_read(is, h_in);
  double nbins_in = h_in.nbins();
  double max_in = h_in.max();
  double cn_in = h_in.counts(3);
  double error = vcl_fabs(nbins_in-nbinsd)+
    vcl_fabs(max-max_in)+vcl_fabs(cn-cn_in);
  TEST_NEAR("histogram binary io", error, 0.0, 0.001);
  vpl_unlink("./temp.bin");

  // joint histogram
  double nbinsjd = jh.nbins();
  double pj = jh.p(1,1);
  double rangej = jh.range();
  vsl_b_ofstream jos("./temp.bin");
  vsl_b_write(jos, jh);
  jos.close();

  vsl_b_ifstream jis("./temp.bin");
  bsta_joint_histogram<double> jh_in;
  vsl_b_read(jis, jh_in);
  double nbinsj_in = jh_in.nbins();
  double rangej_in = jh_in.range();
  double pj_in = jh_in.p(1,1);
  double jerror = vcl_fabs(nbinsj_in-nbinsjd)+
    vcl_fabs(rangej-rangej_in)+vcl_fabs(pj-pj_in);

  TEST_NEAR("joint_histogram binary io", jerror, 0.0, 0.001);
  vpl_unlink("./temp.bin");

  //Test smart pointer
  bsta_histogram_sptr hptr = new bsta_histogram<double>(10.0, 10);
  bsta_histogram<double>* dcast = dynamic_cast<bsta_histogram<double>*>(hptr.ptr());
  bsta_histogram<float>* fcast = dynamic_cast<bsta_histogram<float>*>(hptr.ptr());
  TEST("dynamic cast histogram", dcast&&!fcast, true);
}

TESTMAIN(test_bsta_histogram);
