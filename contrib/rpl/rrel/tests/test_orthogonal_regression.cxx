#include <vcl_iostream.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

#include <testlib/testlib_test.h>

#include <rrel/rrel_orthogonal_regression.h>

bool close(double,double);

static void test_orthogonal_regression()
{
  //  z = 0.02 x - 0.1 y + 10.0
  double a0 = 0.02, a1 = -0.1, a2 = 10.0;
  vnl_vector<double> true_params(4, a0,a1,-1.0,a2);
  vnl_vector<double> par(4);
  vnl_vector<double> norm_vect(3, true_params[0],true_params[1],true_params[2]);
  true_params /= norm_vect.two_norm();  //  4 component vector
  norm_vect /= norm_vect.two_norm();    //  3 component normal only
  const unsigned int num_pts=7;

  //  Build LinearRegression objects exercising both constructors and
  //  the two different options for the first constructor.
  vcl_vector<double> error( num_pts );
  error[0]=-0.001;
  error[1]=0;
  error[2]=0;
  error[3]=-0.0025;
  error[4]=0.007;
  error[5]=0;
  error[6]=-0.004;


  vcl_vector< vnl_vector<double> > pts(num_pts);
  vnl_vector<double> p(3);

  p.x() = 1.0;  p.y()=-0.5; p.z() = a0 * p.x() + a1* p.y() + a2;
  p += error[0] * norm_vect;
  pts[0] = p;

  p.x() = 2.0;  p.y()=4.0;  p.z() = a0 * p.x() + a1* p.y() + a2;
  p += error[1] * norm_vect;
  pts[1] = p;

  p.x()= 3.0;   p.y()=1.0;  p.z() = a0 * p.x() + a1* p.y() + a2;
  p += error[2] * norm_vect;
  pts[2] = p;

  p.x()= -2.0;  p.y()=3.0;  p.z() = a0 * p.x() + a1* p.y() + a2;
  p += error[3] * norm_vect;
  pts[3] = p;

  p.x()= 2.0;   p.y()=4.0;  p.z() = a0 * p.x() + a1* p.y() + a2;
  p += error[4] * norm_vect;
  pts[4] = p;

  p.x()= 5.0;   p.y()=-4.0; p.z() = a0 * p.x() + a1* p.y() + a2;
  p += error[5] * norm_vect;
  pts[5] = p;

  p.x()= 3.0;   p.y()=-2.0; p.z() = a0 * p.x() + a1* p.y() + a2;
  p += error[6] * norm_vect;
  pts[6] = p;

  //
  //  The first set of tests are for the constructor, and parameter access methods.
  //
  testlib_test_begin( "ctor 1" );
  rrel_estimation_problem * lr1 = new rrel_orthogonal_regression( pts );
  testlib_test_perform( lr1 != 0 );


  testlib_test_begin( "num_points_to_instantiate (1)" );
  testlib_test_perform( lr1->num_samples_to_instantiate() == 3 );
  testlib_test_begin( "num_points_to_instantiate (3)" );
  testlib_test_perform( lr1->num_samples() == num_pts );
  testlib_test_begin( "dtor (1)" );
  delete lr1;
  testlib_test_perform( true );


  //
  //  Test the residuals function.
  //
  vcl_vector<double> residuals( pts.size() );
  lr1 = new rrel_orthogonal_regression( pts );

  testlib_test_begin( "residuals" );
  lr1->compute_residuals( true_params, residuals );
  bool ok = (residuals.size() == num_pts);
  for ( unsigned int i=0; i<residuals.size() && ok; ++ i )
    ok = close( residuals[i], error[i] );
  testlib_test_perform( ok );

  //
  //  Test the fit from minimal set function.
  //
  vcl_vector< int > pts_indices(3);
  pts_indices[0] = 1;
  pts_indices[1] = 2;
  pts_indices[2] = 3;
  vnl_vector<double> params;
  ok = lr1->fit_from_minimal_set( pts_indices, params );
  vnl_vector<double> diff1( params - true_params );
  vnl_vector<double> diff2( params + true_params );
  double err;
  if ( diff1.two_norm() < diff2.two_norm() )
    err = diff1.two_norm();
  else
    err = diff2.two_norm();

  testlib_test_begin( "fit_from_minimal_set" );
  testlib_test_perform( ok && err <1e-2 );
#if 0
  vcl_cout << " estimated params: " << params << vcl_endl
           << " true params: " << true_params << vcl_endl
           << " error : " << err << vcl_endl;
#endif

  //
  //  Test the weighted least squares function.
  //
  vcl_vector<double> wgts( num_pts );
  vnl_matrix<double> cofact;

  // Make weights so that the estimation is singular.
  wgts[0] = 0;   wgts[1] = 1;   wgts[2] = 0;    wgts[3] = 0;
  wgts[4] = 0;   wgts[5] = 0;   wgts[6] = 0;
  testlib_test_begin( "weighted_least_squares_fit (singular)" );
  testlib_test_perform( !lr1->weighted_least_squares_fit( par, cofact, &wgts ) );

  // Ok.  This one should work.
  ok = lr1->weighted_least_squares_fit( par, cofact );

  diff1 = par - true_params;
  diff2 = par + true_params;
  if ( diff1.two_norm() < diff2.two_norm() )
    err = diff1.two_norm();
  else
    err = diff2.two_norm();

  testlib_test_begin( "weighted_least_squares_fit (ok) ");
  testlib_test_perform( ok && err <1e-2 );
#if 0
  vcl_cout << " estimated params: " << par << vcl_endl
           << " true params: " << true_params << vcl_endl
           << " error : " << err << vcl_endl;
#endif

  delete lr1;
}

TESTMAIN(test_orthogonal_regression);
