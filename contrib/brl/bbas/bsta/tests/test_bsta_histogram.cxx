//:
// \file
#include <testlib/testlib_test.h>
#include <vcl_cmath.h>
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

#include <bsta/bsta_gauss_sf1.h>

static void test_int_hist()
{
  bsta_int_histogram_1d h1d(50);
  vcl_cout << "Num 1d bins "  << h1d.get_nbins() << '\n';
  TEST("Test No Bins 1d", h1d.get_nbins(), 50);
  for (unsigned k = 0; k<50; ++k)
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
  vcl_ofstream of("joint_out.out");
  jh.print_to_m(of);
  of.close();

  bsta_joint_histogram<double> new_jh(range, bins);
  for (int a =0; a<bins; a++, va+=delta) {
    double avg, var;
    jh.avg_and_variance_bin_for_row_a(a, avg, var);
    vcl_cout << "avg: " << avg << " var: " << var << vcl_endl;

    bsta_gauss_sf1 g(static_cast<float>(avg), static_cast<float>(var));
    for (int b = 0; b < bins; b++) {
      float b_val = g.prob_density(static_cast<float>((b+1)*delta));
      new_jh.upcount(static_cast<float>((a+1)*delta), 0.0f, static_cast<float>((b+1)*delta), b_val);
    }
  }
  vcl_ofstream of2("joint_out_new.out");
  new_jh.print_to_m(of2);
  of2.close();


  TEST_NEAR("test joint histogram uniform distribution entropy", jent, 31.0/4, 1e-9);
  test_int_hist();

  // Test joint histogram with different range and granularity in a and b
  double min_a = -10.0, max_a = 10.0, min_b = -20.0, max_b = 20.0;
  unsigned nbins_a = 11, nbins_b = 21, r0 = 5, c0 = 10;
  bsta_joint_histogram<double> jh_m(min_a, max_a, nbins_a,
                                    min_b, max_b, nbins_b);
  jh_m.upcount(0.0, 0.5, 0.0, 0.5);
  double vol = jh_m.volume();
  double pv = jh_m.p(0.0, 0.0);
  double pb = jh_m.p(r0, c0);
  TEST_NEAR("test min, max joint histogram ", (vol-1.0)+(pv-pb), 0.0, 0.0001);
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

  vsl_b_ofstream sos("./sptr_temp.bin");
  bsta_histogram_sptr hptr = new bsta_histogram<double>(h);
  vsl_b_write(sos, hptr);
  sos.close();
  vsl_b_ifstream sis("./sptr_temp.bin");
  bsta_histogram_sptr hptr_in = 0;
  vsl_b_read(sis, hptr_in);
  TEST("histogram sptr read", hptr_in!=0, true);
  if (hptr_in!=0) {
    bsta_histogram<double>* hp =
      static_cast<bsta_histogram<double>*>(hptr_in.ptr());
    nbins_in = hp->nbins();
    max_in = hp->max();
    cn_in = hp->counts(3);
    error = vcl_fabs(nbins_in-nbinsd)+
      vcl_fabs(max-max_in)+vcl_fabs(cn-cn_in);
    TEST_NEAR("histogram pointer binary io", error, 0.0, 0.001);
  }
  vpl_unlink("./sptr_temp.bin");
  // joint histogram
  double nbinsjd = jh.nbins();
  unsigned ia= 1, ib = 1;
  double pj = jh.p(ia,ib);
  double rangej = jh.range();
  vsl_b_ofstream jos("./temp.bin");
  vsl_b_write(jos, jh);
  jos.close();

  vsl_b_ifstream jis("./temp.bin");
  bsta_joint_histogram<double> jh_in;
  vsl_b_read(jis, jh_in);
  double nbinsj_in = jh_in.nbins();
  double rangej_in = jh_in.range();
  double pj_in = jh_in.p(ia,ib);
  double jerror = vcl_fabs(nbinsj_in-nbinsjd)+
    vcl_fabs(rangej-rangej_in)+vcl_fabs(pj-pj_in);

  TEST_NEAR("joint_histogram binary io", jerror, 0.0, 0.001);
  vpl_unlink("./temp.bin");

  //: test vrml
  vcl_ofstream jos_vrml("./temp.wrl");
  jh.print_to_vrml(jos_vrml);
  jos_vrml.close();
  vpl_unlink("./temp.wrl");

  //Test smart pointer
  hptr = new bsta_histogram<double>(10.0, 10);
  bsta_histogram<double>* dcast = dynamic_cast<bsta_histogram<double>*>(hptr.ptr());
  bsta_histogram<float>* fcast = dynamic_cast<bsta_histogram<float>*>(hptr.ptr());
  TEST("dynamic cast histogram", dcast&&!fcast, true);

  //Test Mutual Information
  bsta_joint_histogram<double> joint_histogram_mi(2,2);
  joint_histogram_mi.upcount(1,1,1,1);
  joint_histogram_mi.upcount(1,3,2,3);
  joint_histogram_mi.upcount(2,2,1,2);
  joint_histogram_mi.upcount(2,4,2,4);
  double mutual_information_error = joint_histogram_mi.mutual_information() - double(0.005802149014346);
  TEST_NEAR("Mutual Information",mutual_information_error,0.0,0.0001);
}

TESTMAIN(test_bsta_histogram);
