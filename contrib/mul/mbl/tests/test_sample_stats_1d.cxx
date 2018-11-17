// Test program fpr test_sample_stats_1d
#include <iostream>
#include <algorithm>
#include <cmath>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_sample_stats_1d.h>


//=============================================================================
void test_original()
{
  mbl_sample_stats_1d stats;
  stats.set_use_mvue(false);

  TEST("Empty N.obs",stats.n_samples(),0);
  TEST("Empty mean",stats.mean(),0.0);

  for (int i=0;i<5;++i)
    stats.add_sample(i);

  TEST("n_samples()",stats.n_samples(),5);
  TEST_NEAR("mean()",stats.mean(),2,1e-6);
  TEST_NEAR("mean_of_absolutes()",stats.mean_of_absolutes(),2,1e-6);
  TEST_NEAR("sum()",stats.sum(),10,1e-6);
  TEST_NEAR("min()",stats.min(),0,1e-6);
  TEST_NEAR("max()",stats.max(),4,1e-6);
  TEST_NEAR("median()",stats.median(),2.0,1e-6);
  TEST_NEAR("sum_squares()",stats.sum_squares(),30,1e-6);
  TEST_NEAR("rms()",stats.rms(),std::sqrt(6.0),1e-6);

  // check mean_of_absolutes
  mbl_sample_stats_1d stats_moa = stats;
  stats_moa.add_sample(-4);
  TEST_NEAR("mean()",stats_moa.mean(),1,1e-6);
  TEST_NEAR("mean_of_absolutes()",stats_moa.mean_of_absolutes(),14.0/6.0,1e-6);

  // check assignment
  mbl_sample_stats_1d stats_i = stats;
  TEST("Equality operator",stats_i,stats);

  // check construct with samples
  std::vector<double> samples;
  samples.reserve(5);
for (int i=0;i<5;++i)
    samples.push_back(i);
  mbl_sample_stats_1d stats_c(samples);
  stats_c.set_use_mvue(false);
  TEST("Equality operator on constructed with samples",stats_c,stats);

  // check adding stats together
  mbl_sample_stats_1d stats2;
  stats.set_use_mvue(false);
  for (int i=5;i<10;++i)
    stats2.add_sample(i);

  stats += stats2;

  TEST("n_samples()",stats.n_samples(),10);
  TEST_NEAR("mean()",stats.mean(),4.5,1e-6);
  TEST_NEAR("sum()",stats.sum(),45,1e-6);
  TEST_NEAR("min()",stats.min(),0,1e-6);
  TEST_NEAR("max()",stats.max(),9,1e-6);
  {
    TEST_NEAR("median() - 1",stats.median(),4.5,1e-6);

    std::vector<double> data;
    data.push_back( 4 );
    data.push_back( 4 );
    data.push_back( 4 );
    data.push_back( 4 );
    data.push_back( 4 );
    data.push_back( 3.33333 );
    data.push_back( 3.33333 );
    data.push_back( 4 );
    data.push_back( 4 );
    data.push_back( 3.33333 );
    data.push_back( 3.33333 );
    data.push_back( 4 );
    data.push_back( 4 );
    data.push_back( 4 );
    data.push_back( 4 );
    data.push_back( 4 );
    data.push_back( 4.66667 );
    data.push_back( 4.66667 );
    data.push_back( 4.66667 );
    data.push_back( 4.66667 );

    mbl_sample_stats_1d stats_med( data );
    TEST_NEAR("median() - 2",stats_med.median(),4,1e-6);

    TEST_NEAR("nth_percentile()",stats_med.nth_percentile(50),4,1e-6);
  }

  TEST_NEAR("nth_percentile()",stats.nth_percentile(10),0,1e-6);
  TEST_NEAR("nth_percentile()",stats.nth_percentile(20),1,1e-6);
  TEST_NEAR("nth_percentile()",stats.nth_percentile(30),2,1e-6);
  TEST_NEAR("nth_percentile()",stats.nth_percentile(40),3,1e-6);
  TEST_NEAR("nth_percentile()",stats.nth_percentile(50),4,1e-6);
  TEST_NEAR("nth_percentile()",stats.nth_percentile(60),5,1e-6);
  TEST_NEAR("nth_percentile()",stats.nth_percentile(70),6,1e-6);
  TEST_NEAR("nth_percentile()",stats.nth_percentile(80),7,1e-6);
  TEST_NEAR("nth_percentile()",stats.nth_percentile(90),8,1e-6);
  TEST_NEAR("nth_percentile()",stats.nth_percentile(100),9,1e-6);
  TEST_NEAR("sum_squares()",stats.sum_squares(),285,1e-6);

  // test higher order statistics
  mbl_sample_stats_1d stats3;
  stats3.set_use_mvue(true);
  stats3.add_sample(10);
  stats3.add_sample(10);
  stats3.add_sample(-3);
  stats3.add_sample(-1);

  double sd=6.9761498;
  double var=48.6666666;

  TEST_NEAR("sd()",stats3.sd(),sd,1e-6);
  TEST_NEAR("variance()",stats3.variance(),var,1e-6);

  // more checks for median
  mbl_sample_stats_1d stats4;
  stats4.add_sample(0);
  TEST_NEAR("median()",stats4.median(),0,1e-6);
  stats4.add_sample(1);
  TEST_NEAR("median()",stats4.median(),0.5,1e-6);
  stats4.add_sample(100);
  TEST_NEAR("median()",stats4.median(),1,1e-6);
  stats4.add_sample(101);
  TEST_NEAR("median()",stats4.median(),50.5,1e-6);
  stats4.add_sample(102);
  TEST_NEAR("median()",stats4.median(),100,1e-6);

  // test convenience functions
  std::vector<double> vec5;
  vec5.push_back(1);
  vec5.push_back(2);
  vec5.push_back(3);
  vec5.push_back(4);
  vec5.push_back(5);

  std::vector<double> mask1;
  mask1.push_back(0);
  mask1.push_back(1);
  mask1.push_back(0);
  mask1.push_back(0);
  mask1.push_back(1);
  mbl_sample_stats_1d stats5(mbl_apply_mask(vec5,mask1));
  TEST_NEAR("Masked stats correct",stats5.mean(),0.5*(2+5),0.001);
  std::vector<int> mask2;
  mask2.push_back(1);
  mask2.push_back(0);
  mask2.push_back(0);
  mask2.push_back(0);
  mask2.push_back(1);
  mbl_sample_stats_1d stats6(mbl_apply_mask(vec5,mask2));
  TEST_NEAR("Masked stats correct",stats6.mean(),0.5*(1+5),0.001);
}


//=============================================================================
void test_quantile()
{
  // using "{}" blocks for higher autonomy and reusablility of variables
  {
    std::cout << "test_quantile(): odd number of samples (nsamples=5)\n";
    constexpr unsigned ns = 5;
    mbl_sample_stats_1d stats;
    for (unsigned int i=0; i<ns; ++i)
      stats.add_sample(i);

    constexpr unsigned nq = 10; // will actually calculate nq+1 quantiles
    std::vector<double> quantiles;
    for (unsigned j=0; j<=nq; ++j)
    {
      double q = static_cast<double>(j)/static_cast<double>(nq);
      quantiles.push_back(stats.quantile(q));
    }

    TEST("quantile(0.0)==ordered_sample[0]?", quantiles[0], stats.samples()[0]);
    TEST("quantile(1.0)==ordered_sample[n-1]?", quantiles[nq], stats.samples()[ns-1]);
    TEST("quantile(0.5)==ordered_sample[(n-1)/2]?", quantiles[nq/2], stats.samples()[(ns-1)/2]);
    TEST("median()==quantile(0.5)?", stats.median(), stats.quantile(0.5));
  }
  {
    std::cout << "test_quantile(): even number of samples (nsamples=6)\n";
    constexpr unsigned ns = 6;
    mbl_sample_stats_1d stats;
    for (unsigned int i=0; i<ns; ++i)
      stats.add_sample(i);

    constexpr unsigned nq = 10; // will actually calculate nq+1 quantiles
    std::vector<double> quantiles;
    for (unsigned j=0; j<=nq; ++j)
    {
      double q = static_cast<double>(j)/static_cast<double>(nq);
      quantiles.push_back(stats.quantile(q));
    }

    TEST("quantile(0.0)==ordered_sample(0)?", quantiles[0]==stats.samples()[0], true);
    TEST("quantile(1.0)==ordered_sample(n-1)?", quantiles[nq]==stats.samples()[ns-1], true);

    {
      std::vector<double> samples_cpy = stats.samples();
      std::sort( samples_cpy.begin() ,samples_cpy.end() );
      double temp = (samples_cpy[ns/2 -1] + samples_cpy[ns/2])/2.0;
      TEST("quantile(0.5)==mean of 2 adj samples?", quantiles[nq/2]==temp, true);
    }
    TEST("median()==quantile(0.5)?", stats.median()==stats.quantile(0.5), true);
  }
}


//=============================================================================
void test_sample_stats_1d()
{
  std::cout << "*****************************\n"
           << " Testing mbl_sample_stats_1d\n"
           << "*****************************\n";

  test_original();
  test_quantile();
}


//=============================================================================
TESTMAIN(test_sample_stats_1d);
