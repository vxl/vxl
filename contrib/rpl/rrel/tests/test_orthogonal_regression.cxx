#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_matrix.h>

#include <testlib/testlib_test.h>

#include <rrel/rrel_orthogonal_regression.h>

bool close(double,double);

static void test_orthogonal_regression()
{
  //  z = 0.02 x - 0.1 y + 10.0
  double a0 = 0.02, a1 = -0.1, a2 = 10.0;
  vnl_double_4 true_params(a0,a1,-1.0,a2);
  vnl_vector<double> par;
  vnl_double_3 norm_vect(true_params[0],true_params[1],true_params[2]);
  true_params /= norm_vect.two_norm();  //  4 component vector
  norm_vect /= norm_vect.two_norm();    //  3 component normal only
  constexpr unsigned int num_pts = 7;

  //  Build LinearRegression objects exercising both constructors and
  //  the two different options for the first constructor.
  std::vector<double> error( num_pts );
  error[0]=-0.001;
  error[1]=0;
  error[2]=0;
  error[3]=-0.0025;
  error[4]=0.007;
  error[5]=0;
  error[6]=-0.004;


  std::vector< vnl_vector<double> > pts(num_pts);
  vnl_double_3 p;

  p[0] = 1.0;  p[1]=-0.5; p[2] = a0 * p[0] + a1* p[1] + a2;
  p += error[0] * norm_vect;
  pts[0] = p.as_ref();

  p[0] = 2.0;  p[1]=4.0;  p[2] = a0 * p[0] + a1* p[1] + a2;
  p += error[1] * norm_vect;
  pts[1] = p.as_ref();

  p[0]= 3.0;   p[1]=1.0;  p[2] = a0 * p[0] + a1* p[1] + a2;
  p += error[2] * norm_vect;
  pts[2] = p.as_ref();

  p[0]= -2.0;  p[1]=3.0;  p[2] = a0 * p[0] + a1* p[1] + a2;
  p += error[3] * norm_vect;
  pts[3] = p.as_ref();

  p[0]= 2.0;   p[1]=4.0;  p[2] = a0 * p[0] + a1* p[1] + a2;
  p += error[4] * norm_vect;
  pts[4] = p.as_ref();

  p[0]= 5.0;   p[1]=-4.0; p[2] = a0 * p[0] + a1* p[1] + a2;
  p += error[5] * norm_vect;
  pts[5] = p.as_ref();

  p[0]= 3.0;   p[1]=-2.0; p[2] = a0 * p[0] + a1* p[1] + a2;
  p += error[6] * norm_vect;
  pts[6] = p.as_ref();

  //
  //  The first set of tests are for the constructor, and parameter access methods.
  //
  rrel_estimation_problem * lr1 = new rrel_orthogonal_regression( pts );
  TEST("ctor 1" , !lr1, false);

  TEST("num_points_to_instantiate (1)" , lr1->num_samples_to_instantiate(), 3);
  TEST("num_points_to_instantiate (3)" , lr1->num_samples(), num_pts);
  testlib_test_begin( "dtor (1)" );
  delete lr1;
  testlib_test_perform( true );


  //
  //  Test the residuals function.
  //
  std::vector<double> residuals( pts.size() );
  lr1 = new rrel_orthogonal_regression( pts );

  lr1->compute_residuals( true_params.as_vector(), residuals );
  bool ok = (residuals.size() == num_pts);
  for ( unsigned int i=0; i<residuals.size() && ok; ++ i )
    ok = close( residuals[i], error[i] );
  TEST("residuals", ok, true);

  //
  //  Test the fit from minimal set function.
  //
  std::vector< int > pts_indices(3);
  pts_indices[0] = 1;
  pts_indices[1] = 2;
  pts_indices[2] = 3;
  vnl_vector<double> params;
  ok = lr1->fit_from_minimal_set( pts_indices, params );
  vnl_double_4 diff1 = params - true_params;
  vnl_double_4 diff2 = params + true_params;
  double err;
  if ( diff1.two_norm() < diff2.two_norm() )
    err = diff1.two_norm();
  else
    err = diff2.two_norm();

  TEST("fit_from_minimal_set" , ok && err <1e-2, true);
#if 0
  std::cout << " estimated params: " << params << std::endl
           << " true params: " << true_params << std::endl
           << " error : " << err << std::endl;
#endif

  //
  //  Test the weighted least squares function.
  //
  std::vector<double> wgts( num_pts );
  vnl_matrix<double> cofact;

  // Make weights so that the estimation is singular.
  wgts[0] = 0;   wgts[1] = 1;   wgts[2] = 0;    wgts[3] = 0;
  wgts[4] = 0;   wgts[5] = 0;   wgts[6] = 0;
  TEST("weighted_least_squares_fit (singular)" , !lr1->weighted_least_squares_fit( par, cofact, &wgts ), true);

  // Ok.  This one should work.
  ok = lr1->weighted_least_squares_fit( par, cofact );

  diff1 = vnl_double_4(par) - true_params;
  diff2 = vnl_double_4(par) + true_params;
  if ( diff1.two_norm() < diff2.two_norm() )
    err = diff1.two_norm();
  else
    err = diff2.two_norm();

  TEST("weighted_least_squares_fit (ok) ", ok && err <1e-2, true);
#if 0
  std::cout << " estimated params: " << par << std::endl
           << " true params: " << true_params << std::endl
           << " error : " << err << std::endl;
#endif

  delete lr1;
}

TESTMAIN(test_orthogonal_regression);
