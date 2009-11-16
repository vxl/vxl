//:
// \file
#include <testlib/testlib_test.h>
#include <bsta/bsta_beta.h>
#include <vcl_iostream.h>

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
}

TESTMAIN(test_beta);
