#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <rrel/rrel_homography2d_est.h>

static void test_homography2d_est()
{
  vnl_double_3x3 H(0.0);
  vnl_matrix<double> cofact;
  vcl_vector <int> indices;
  vcl_vector <vnl_vector<double> > p,q;
  vnl_vector<double> param(9,0.0);
  vnl_vector<double> true_param(9,0.0);
  int n;
  int i,j;
  const double pi = vnl_math::pi;
  const double tol = 1e-8;
  vnl_double_3 t(0,0,1);

  p.push_back(t.as_ref());

  //first 4 points are collinear.
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
  n = p.size();
  q.resize(n);
  indices.resize(4);

  // Test points to instantiate
  {
    rrel_homography2d_est homo_est(p,p);
    TEST("Points to instantiate", homo_est.num_samples_to_instantiate(), 4);
  }
  // translation only
  H(0,0) = H(1,1) = 1;
  H(0,2) = -4;
  H(1,2) = 2;
  H(2,2) = 1;
  {
    for (i=0;i<3;i++)
      for (j=0;j<3;j++)
        true_param[i*3+j] = H(i,j);
    true_param /= true_param.two_norm();
    for (i=0;i<n;i++)
      q[i] = H *p[i];
    rrel_homography2d_est homo_est(p,q);
    indices[0] = 10; indices[1]=1; indices[2]=2; indices[3]=3;
    bool ret = homo_est.fit_from_minimal_set(indices, param);
    TEST("Degeneracy Case", ret, false);

    indices[0] = 0; indices[1]=1; indices[2]=8; indices[3]=10;
    homo_est.fit_from_minimal_set(indices, param);
    if (param[0]<0)  param = -param;
    param /= param.two_norm();
    TEST("(Translation) minimal-set estimation", (param-true_param).two_norm() < tol, true);

    homo_est.weighted_least_squares_fit(param, cofact, NULL);
    if (param[0]<0)  param = -param;
    param /= param.two_norm();
    TEST("(Translation) Weighted Least Squares", (param-true_param).two_norm() < tol, true);
  }

  //similarity transform
  H(1,1) = H(0,0) = 2*vcl_cos(pi/3);
  H(0,1) = -2*vcl_sin(pi/3);
  H(1,0) = -H(0,1);
  {
    for (i=0;i<3;i++)
      for (j=0;j<3;j++)
        true_param[i*3+j] = H(i,j);
    true_param /= true_param.two_norm();
    for (i=0;i<n;i++)
      q[i] = H *p[i];

    rrel_homography2d_est homo_est(p,q);
    indices[0] = 0; indices[1]=2; indices[2]=8; indices[3]=10;
    homo_est.fit_from_minimal_set(indices, param);
    if (param[0]<0)  param = -param;
    param /= param.two_norm();
    TEST("(Similarity) minimal-set estimation", (param-true_param).two_norm() < tol, true);

    homo_est.weighted_least_squares_fit(param, cofact, NULL);
    if (param[0]<0)  param = -param;
    param /= param.two_norm();
    TEST("(Similarity) Weighted Least Squares", (param-true_param).two_norm() < tol, true);
  }

  // affine transform
  H(0,1) = -5; H(1,1) = -1.5;
  {
    for (i=0;i<3;i++)
      for (j=0;j<3;j++)
        true_param[i*3+j] = H(i,j);
    true_param /= true_param.two_norm();
    for (i=0;i<n;i++)
      q[i] = H *p[i];

    rrel_homography2d_est homo_est(p,q);
    indices[0] = 2; indices[1]=5; indices[2]=8; indices[3]=10;
    homo_est.fit_from_minimal_set(indices, param);
    if (param[0]<0)  param = -param;
    param /= param.two_norm();
    TEST("(Affine) minimal-set estimation", (param-true_param).two_norm() < tol, true);

    homo_est.weighted_least_squares_fit(param, cofact, NULL);
    if (param[0]<0)  param = -param;
    param /= param.two_norm();
    TEST("(Affine) Weighted Least Squares", (param-true_param).two_norm() < tol, true);
  }

  // projective transform
  H(2,0) = 0.5; H(2,1) = -2;
  {
    for (i=0;i<3;i++)
      for (j=0;j<3;j++)
        true_param[i*3+j] = H(i,j);
    true_param /= true_param.two_norm();
    for (i=0;i<n;i++)
      q[i] = H *p[i];

    rrel_homography2d_est homo_est(p,q);
    indices[0] = 0; indices[1]=2; indices[2]=8; indices[3]=10;
    homo_est.fit_from_minimal_set(indices, param);
    if (param[0]<0)  param = -param;
    param /= param.two_norm();
    TEST("(Projective) minimal-set estimation", (param-true_param).two_norm() < tol, true);

    homo_est.weighted_least_squares_fit(param, cofact);
    if (param[0]<0)  param = -param;
    param /= param.two_norm();
    TEST("(Projective) Weighted Least Squares", (param-true_param).two_norm() < tol, true);

    // degenerate
    vcl_vector <double> wgts(n,0.0);
    for (i=0;i<5;i++)
      wgts[i] = 1.0;
    bool ret = homo_est.weighted_least_squares_fit(param, cofact, &wgts);
    TEST("Degeneracy of Projective Weighted Least Squares", ret, false);
  }
}

TESTMAIN(test_homography2d_est);
