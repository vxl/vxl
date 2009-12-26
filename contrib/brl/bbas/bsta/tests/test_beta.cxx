//:
// \file
#include <testlib/testlib_test.h>
#include <bsta/bsta_beta.h>
#include <bsta/bsta_histogram.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

//: Test bsta beta distribution
void test_beta()
{
  bsta_beta<double> beta1(2.0,2.0);
  double val = beta1.prob_density(0.0);
  TEST_NEAR("beta distribution of (2,2) at 0.0"   , val,  0.0 ,1e-06);
  val = beta1.prob_density(1.0);
  TEST_NEAR("beta distribution of (2,2) at 1.0"   , val,  0.0 ,1e-06);
  val = beta1.prob_density(0.5);
  TEST_NEAR("beta distribution of (2,2) at 0.5"   , val,  1.5 ,1e-06);

  bsta_beta<double> beta2(2.0,5.0);
  val = beta2.prob_density(0.0);
  TEST_NEAR("beta distribution of (2,5) at 0.0"   , val,  0.0 ,1e-06);
  val = beta2.prob_density(1.0);
  TEST_NEAR("beta distribution of (2,5) at 1.0"   , val,  0.0 ,1e-06);
  val = beta2.prob_density(0.2);
  TEST_NEAR("beta distribution of (2,5) at 0.2"   , val,  2.4576 ,1e-06);

  // test cumulative distribution function
  bsta_beta<double> beta3(2,5);
  double v1 = beta3.cum_dist_funct(0);
  TEST_NEAR("cumulative distribution function at x=0", v1, 0.0 ,1e-06);

  double v2 = beta3.cum_dist_funct(1);
  TEST_NEAR("cumulative distribution function at x=1", v2, 1.0 ,1e-06);

  double ax = beta3.cum_dist_funct(0.2);
  bsta_beta<double> betat(5,2);
  double ay = 1 - betat.cum_dist_funct(0.8);
  TEST_NEAR("cumulative distribution function  equal at x,(a,b) and (1-x),(b,a)", ax, ay ,1e-06);

  // read the samples from a txt file,
  // extract the parameters from a set of data
  vcl_ifstream is("beta_distr_2_2.txt");
  double alpha=2, beta=2;
  vcl_vector<double> samples;
  while (!is.eof()) {
    double sample;
    is >> sample;
    samples.push_back(sample);
  }

  bsta_beta<double> beta4(samples); vcl_cout << beta4;
  TEST_NEAR("extracting alpha and beta from samples (2,2) - alpha", beta4.alpha(), alpha ,1e-01);
  TEST_NEAR("extracting alpha and beta from samples (2,2) - beta ", beta4.beta(), beta ,1e-01);
}

TESTMAIN(test_beta);
