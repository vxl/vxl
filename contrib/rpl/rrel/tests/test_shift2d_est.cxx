#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <rrel/rrel_shift2d_est.h>

static void test_shift2d_est()
{
  vcl_vector <vnl_vector<double> > p;
  vnl_double_3 t(0,0,1);

  p.push_back(t.as_ref());

  t(0) = 2; t(1) = 5;
  p.push_back(t.as_ref());

  t(0) = 4; t(1) = 9;
  p.push_back(t.as_ref());

  t(0) = -1; t(1) = -1;
  p.push_back(t.as_ref());

  t(0) = -3; t(1) = -5;
  p.push_back(t.as_ref());

  t(0) = -9; t(1) = .5;
  p.push_back(t.as_ref());

  t(0) = 5; t(1) = -5.678;
  p.push_back(t.as_ref());

  t(0) = 5; t(1) = -5.678; t(2) = 3;
  p.push_back(t.as_ref());

  t(0) = 0.4; t(1) = 0.894; t(2) = 0.1;
  p.push_back(t.as_ref());

  t(0) = 500; t(1) = -100; t(2) = 100;
  p.push_back(t.as_ref());

  t(0) = -20; t(1) = -20; t(2) = 1;
  p.push_back(t.as_ref());

  t(0) = 4; t(1) = 0.02; t(2) = 1.5;
  p.push_back(t.as_ref());

  t(0) = 2.345; t(1) = -10; t(2) = 1;
  p.push_back(t.as_ref());

  t(0) = 8.9e-4; t(1) = -3.1e-4; t(2) = -1e-4;
  p.push_back(t.as_ref());

  t(0) = -10; t(1) = 40; t(2) = 1;
  p.push_back(t.as_ref());

  // ----------------------------------------------------------------
  int n = p.size();
  vcl_vector<vnl_vector<double> > q(n);

  // Test points to instantiate
  {
    rrel_shift2d_est shift_est(p,p);
    TEST("Points to instantiate", shift_est.num_samples_to_instantiate(), 1);
  }
  // translation only
  vnl_double_3x3 H(0.0);
  H(0,0) = H(1,1) = 1;
  H(0,2) = -4;
  H(1,2) = 2;
  H(2,2) = 1;
  {
    vnl_vector<double> true_param(2,0.0);
    true_param[0] = H(0,2);
    true_param[1] = H(1,2);
    true_param /= true_param.two_norm();
    for (int i=0; i<n; i++)
      q[i] = H *p[i];
    rrel_shift2d_est shift_est(p,q);

    vcl_vector<int> indices(1);
    indices[0] = 1;
    vnl_vector<double> param(2,0.0);
    TEST("fit_from_minimal_set()", shift_est.fit_from_minimal_set(indices, param), true);
    param /= param.two_norm();
    TEST_NEAR("(Translation) minimal-set estimation", (param-true_param).two_norm(), 0.0, 1e-8);

    vnl_matrix<double> cofact;
    TEST("weighted_least_squares_fit()", shift_est.weighted_least_squares_fit(param, cofact, NULL), true);
    param /= param.two_norm();
    TEST_NEAR("(Translation) Weighted Least Squares", (param-true_param).two_norm(), 0.0, 1e-8);
  }
}

TESTMAIN(test_shift2d_est);
