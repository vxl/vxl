//:
// \file
#include <iostream>
#include <cmath>
#include <fstream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_joint_histogram.h>
#include <bsta/bsta_joint_histogram_3d.h>
#include <bsta/bsta_int_histogram_1d.h>
#include <bsta/bsta_int_histogram_2d.h>
#include <vpl/vpl.h>
#include <bsta/io/bsta_io_histogram.h>
#include <vsl/vsl_binary_io.h>

#include <bsta/bsta_histogram_sptr.h>
#include <bsta/bsta_joint_histogram_sptr.h>

#include <bsta/bsta_gauss_sf1.h>

//: Test bsta histograms
void test_bsta_histogram_io()
{
 //=================================================
  // test binary io for histogram classes
  // 1-d histogram
  double range = 128.0;
  int bins = 16;
  double delta = range/bins;
  bsta_histogram<double> h(range, bins);
  double v = 0.0;
  for (int b =0; b<bins; b++, v+=delta)
    h.upcount(v, 1.0);
  std::cout << "Bins\n";

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
  double error = std::fabs(nbins_in-nbinsd)+
    std::fabs(max-max_in)+std::fabs(cn-cn_in);
  TEST_NEAR("histogram binary io", error, 0.0, 0.001);
  vpl_unlink("./temp.bin");
  // smart pointer read write
  vsl_b_ofstream sos("./sptr_temp.bin");
  bsta_histogram_sptr hptr = new bsta_histogram<double>(h);
  vsl_b_write(sos, hptr);
  sos.close();
  vsl_b_ifstream sis("./sptr_temp.bin");
  bsta_histogram_sptr hptr_in = nullptr;
  vsl_b_read(sis, hptr_in);
  TEST("histogram sptr read", hptr_in!=nullptr, true);
  if (hptr_in!=nullptr) {
    auto* hp =
      static_cast<bsta_histogram<double>*>(hptr_in.ptr());
    nbins_in = hp->nbins();
    max_in = hp->max();
    cn_in = hp->counts(3);
    error = std::fabs(nbins_in-nbinsd)+
      std::fabs(max-max_in)+std::fabs(cn-cn_in);
    TEST_NEAR("histogram pointer binary io", error, 0.0, 0.001);
  }
  vpl_unlink("./sptr_temp.bin");

  // joint histogram
  bsta_joint_histogram<double> jh(range, bins);
  double va = 0;
  for (int a =0; a<bins; a++, va+=delta)
  {
    double vb = 0;
    for (int b =0; b<bins; b++, vb+=delta)
      jh.upcount(va, 1.0, vb, 1.0);
  }
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
  double jerror = std::fabs(nbinsj_in-nbinsjd)+
    std::fabs(rangej-rangej_in)+std::fabs(pj-pj_in);

  TEST_NEAR("joint_histogram binary io", jerror, 0.0, 0.001);
  vpl_unlink("./temp.bin");
  // smart pointer read write
  vsl_b_ofstream jsos("./sptr_temp.bin");
  bsta_joint_histogram_sptr jhptr = new bsta_joint_histogram<double>(jh);
  vsl_b_write(jsos, jhptr);
  jsos.close();
  vsl_b_ifstream jsis("./sptr_temp.bin");
  bsta_joint_histogram_sptr jhptr_in = nullptr;
  vsl_b_read(jsis, jhptr_in);
  jsis.close();
  TEST("joint histogram sptr read", jhptr_in!=nullptr, true);
  if (jhptr_in) {
    if (jhptr_in->type_ == bsta_joint_histogram_base::HIST_TYPE_DOUBLE) {
      auto* hdbl =
        dynamic_cast<bsta_joint_histogram<double>*>(jhptr_in.ptr());
      nbinsj_in = hdbl->nbins();
      rangej_in = hdbl->range();
      pj_in = hdbl->p(ia,ib);
      jerror = std::fabs(nbinsj_in-nbinsjd)+
        std::fabs(rangej-rangej_in)+std::fabs(pj-pj_in);
      TEST_NEAR("joint_histogram sptr binary io", jerror, 0.0, 0.001);
    }
    else {
      TEST("joint_histogram sptr binary io", false, true);
    }
  }
  vpl_unlink("./sptr_temp.bin");


  // test bsta_joint_histogram_3d binary io
  bsta_joint_histogram_3d<float> hist_default;
  bsta_joint_histogram_3d<float> hist_cons1(1.0, 10);
  bsta_joint_histogram_3d<float> hist_cons2(1.0, 10,
                                            2.0, 20,
                                            3.0, 30);
  bsta_joint_histogram_3d<float> hist_cons3(0.0, 1.0, 10,
                                            1.0, 2.0, 20,
                                            2.0, 3.0, 30);
  hist_cons3.upcount(0.5f, 1.0f, 1.5f,1.0f, 2.5f, 1.0f);
  bsta_joint_histogram_3d<float> hist_3d(1.0, 10, 1.0, 10, 1.0, 10);
  std::string hpath = "./test_3d_hist_plot.wrl";
  hist_3d.upcount(0.5,1.0, 0.5,1.0, 0.5,1.0);
  hist_3d.upcount(0.25,1.0, 0.25, 1.0, 0.25, 1.0);
  std::ofstream os_3d(hpath.c_str());
  if (os_3d.is_open()){
    hist_3d.print_to_vrml(os_3d);
    os_3d.close();
    vpl_unlink("./test_3d_hist_plot.wrl");
  }

  vsl_b_ofstream os_3do("./temp_3d.bin");
  if (!os_3do)
    return;
  hist_cons2.upcount(0.5f, 1.0f, 1.5f, 1.0f, 2.5f, 1.0f);
  float pw = hist_cons2.p(0.5f,1.5f,2.5f);
  vsl_b_write(os_3do , hist_cons2);
  os_3do.close();

  vsl_b_ifstream is_3d("./temp_3d.bin");
  if (!is_3d)
    return;
  bsta_joint_histogram_3d<float> hd;
  vsl_b_read(is_3d, hd);
  float pin = hd.p(0.5f,1.5f,2.5f);
  float er_3d = std::fabs(pin-pw);
  TEST_NEAR("test 3d hist binary io", er_3d, 0.0f, 0.0001f);
  vpl_unlink("./temp_3d.bin");

  // smart pointer read write
  vsl_b_ofstream j3dsos("./sptr_temp.bin");
  bsta_joint_histogram_3d_sptr jh3dptr =
    new bsta_joint_histogram_3d<float>(hist_cons2);
  vsl_b_write(j3dsos, jh3dptr);
  j3dsos.close();
  vsl_b_ifstream j3dsis("./sptr_temp.bin");
  bsta_joint_histogram_3d_sptr j3dhptr_in = nullptr;
  vsl_b_read(j3dsis, j3dhptr_in);
  j3dsis.close();
  TEST("joint histogram sptr read", j3dhptr_in!=nullptr, true);
  if (j3dhptr_in) {
    if (j3dhptr_in->type_ == bsta_joint_histogram_3d_base::HIST_TYPE_FLOAT) {
      auto* h3ddbl =
        dynamic_cast<bsta_joint_histogram_3d<float>*>(j3dhptr_in.ptr());
      float p3dj_in = h3ddbl->p(0.5f,1.5f,2.5f);
      jerror = std::fabs(p3dj_in-pin);
      TEST_NEAR("joint_histogram_3d sptr binary io", jerror, 0.0, 0.001);
    }
    else {
      TEST("joint_histogram_3d_sptr binary io", false, true);
    }
  }
  vpl_unlink("./sptr_temp.bin");
}

TESTMAIN(test_bsta_histogram_io);
