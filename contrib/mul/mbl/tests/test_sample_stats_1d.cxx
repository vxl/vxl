// Test program fpr test_sample_stats_1d
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <mbl/mbl_sample_stats_1d.h>

void test_sample_stats_1d()
{
  vcl_cout << "*****************************\n"
           << " Testing mbl_sample_stats_1d \n"
           << "*****************************\n";

  mbl_sample_stats_1d stats;
  stats.set_use_mvue(false);

  TEST("Empty N.obs",stats.n_samples()==0,true);
  TEST("Empty mean",stats.mean()==0.0,true);

  for (int i=0;i<5;++i)
    stats.add_sample(i);

  TEST("n_samples()",stats.n_samples()==5,true);
  TEST("mean()",vcl_fabs(stats.mean()-2)<1e-6,true);
  TEST("mean_of_absolutes()",vcl_fabs(stats.mean_of_absolutes()-2)<1e-6,true);
  TEST("sum()",vcl_fabs(stats.sum()-10)<1e-6,true);
  TEST("min()",vcl_fabs(stats.min()-0)<1e-6,true);
  TEST("max()",vcl_fabs(stats.max()-4)<1e-6,true);
  TEST("median()",vcl_fabs(stats.median()-2.0)<1e-6,true);
  TEST("sum_squares()",vcl_fabs(stats.sum_squares()-30)<1e-6,true);
  TEST("rms()",vcl_fabs(stats.rms()-vcl_sqrt(6.0))<1e-6,true);

  // check mean_of_absolutes
  mbl_sample_stats_1d stats_moa = stats;
  stats_moa.add_sample(-4);
  TEST("mean()",vcl_fabs(stats_moa.mean()-1)<1e-6,true);
  TEST("mean_of_absolutes()",vcl_fabs(stats_moa.mean_of_absolutes()-14.0/6.0)<1e-6,true);

  // check assignment
  mbl_sample_stats_1d stats_i = stats;
  TEST("Equality operator",stats_i==stats,true);

  // check construct with samples
  vcl_vector<double> samples;
  samples.push_back(0);
  samples.push_back(1);
  samples.push_back(2);
  samples.push_back(3);
  samples.push_back(4);
  mbl_sample_stats_1d stats_c(samples);
  stats_c.set_use_mvue(false);
  TEST("Equality operator on constructed with samples",stats_c==stats,true);

  // check adding stats together
  mbl_sample_stats_1d stats2;
  stats.set_use_mvue(false);
  for (int i=5;i<10;++i)
    stats2.add_sample(i);

  stats += stats2;

  TEST("n_samples()",stats.n_samples()==10,true);
  TEST("mean()",vcl_fabs(stats.mean()-4.5)<1e-6,true);
  TEST("sum()",vcl_fabs(stats.sum()-45)<1e-6,true);
  TEST("min()",vcl_fabs(stats.min()-0)<1e-6,true);
  TEST("max()",vcl_fabs(stats.max()-9)<1e-6,true);
  TEST("median()",vcl_fabs(stats.median()-4.5)<1e-6,true);
  TEST("nth_percentile()",vcl_fabs(stats.nth_percentile(10)-0)<1e-6,true);
  TEST("nth_percentile()",vcl_fabs(stats.nth_percentile(20)-1)<1e-6,true);
  TEST("nth_percentile()",vcl_fabs(stats.nth_percentile(30)-2)<1e-6,true);
  TEST("nth_percentile()",vcl_fabs(stats.nth_percentile(40)-3)<1e-6,true);
  TEST("nth_percentile()",vcl_fabs(stats.nth_percentile(50)-4)<1e-6,true);
  TEST("nth_percentile()",vcl_fabs(stats.nth_percentile(60)-5)<1e-6,true);
  TEST("nth_percentile()",vcl_fabs(stats.nth_percentile(70)-6)<1e-6,true);
  TEST("nth_percentile()",vcl_fabs(stats.nth_percentile(80)-7)<1e-6,true);
  TEST("nth_percentile()",vcl_fabs(stats.nth_percentile(90)-8)<1e-6,true);
  TEST("nth_percentile()",vcl_fabs(stats.nth_percentile(100)-9)<1e-6,true);
  TEST("sum_squares()",vcl_fabs(stats.sum_squares()-285)<1e-6,true);

  // test higher order statistics
  mbl_sample_stats_1d stats3;
  stats3.set_use_mvue(true);
  stats3.add_sample(10);
  stats3.add_sample(10);
  stats3.add_sample(-3);
  stats3.add_sample(-1);

  double mean=4;
  double sd=6.9761498;
  double var=48.6666666;

  TEST("sd()",vcl_fabs(stats3.sd()-sd)<1e-6,true);
  TEST("variance()",vcl_fabs(stats3.variance()-var)<1e-6,true);

  // more checks for median
  mbl_sample_stats_1d stats4;
  stats4.add_sample(0);
  TEST("median()",vcl_fabs(stats4.median()-0)<1e-6,true);
  stats4.add_sample(1);
  TEST("median()",vcl_fabs(stats4.median()-0.5)<1e-6,true);
  stats4.add_sample(100);
  TEST("median()",vcl_fabs(stats4.median()-1)<1e-6,true);
  stats4.add_sample(101);
  TEST("median()",vcl_fabs(stats4.median()-50.5)<1e-6,true);
  stats4.add_sample(102);
  TEST("median()",vcl_fabs(stats4.median()-100)<1e-6,true);

  // test convenience functions
  vcl_vector<double> vec5;
  vec5.push_back(1);
  vec5.push_back(2);
  vec5.push_back(3);
  vec5.push_back(4);
  vec5.push_back(5);

  vcl_vector<double> mask1;
  mask1.push_back(0);
  mask1.push_back(1);
  mask1.push_back(0);
  mask1.push_back(0);
  mask1.push_back(1);
  mbl_sample_stats_1d stats5(mbl_apply_mask(vec5,mask1));
  TEST_NEAR("Masked stats correct",stats5.mean(),0.5*(2+5),0.001);
  vcl_vector<int> mask2;
  mask2.push_back(1);
  mask2.push_back(0);
  mask2.push_back(0);
  mask2.push_back(0);
  mask2.push_back(1);
  mbl_sample_stats_1d stats6(mbl_apply_mask(vec5,mask2));
  TEST_NEAR("Masked stats correct",stats6.mean(),0.5*(1+5),0.001);
}

TESTMAIN(test_sample_stats_1d);
