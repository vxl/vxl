#include <vcl_iostream.h>

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>

#include <vbl/vbl_test.h>

#include <rrel/rrel_orthogonal_regression.h>


bool close( double x, double y ) { return vnl_math_abs(x-y) < 1.0e-6; }

int
main()
{
  vbl_test_start( "orthogonal regression" );
  vnl_double_3 true_params(3);
  true_params[0] = 10.0;
  true_params[1] = 0.02;
  true_params[2] = -0.1;
  vnl_double_3 a(true_params);
  vnl_double_3 par;
  const int num_pts=7;

  //  Build LinearRegression objects exercising both constructors and
  //  the two different options for the first constructor.
  vcl_vector< vnl_vector<double> > pts(num_pts);
  vcl_vector<double> error( num_pts );

  vnl_double_3 p, q;
  double x, y, z;

  x = 1.0; y=-0.5; error[0]=-0.001;  z= -(a[0]*x + a[1]*y - error[0])/a[2] ;
  p[0] = x; p[1]=y; p[2]=z;  pts[0]=p;

  x = 2.0;  y=4.0;  error[1]=0; z= -(a[0]*x + a[1]*y - error[1])/a[2] ;
  p[0] = x; p[1]=y; p[2]=z;  pts[1]=p;

  x = 3.0;  y=1.0;  error[2]=0; z= -(a[0]*x + a[1]*y - error[2])/a[2] ;
  p[0] = x; p[1]=y; p[2]=z;  pts[2]=p;

  x = -2.0;  y=3.0; error[3]=-0.0025;  z= -(a[0]*x + a[1]*y - error[3])/a[2] ;
  p[0] = x; p[1]=y; p[2]=z;  pts[3]=p;

  x = 2.0;  y=4.0;  error[4]=0.007;  z= -(a[0]*x + a[1]*y - error[4])/a[2] ;
  p[0] = x; p[1]=y; p[2]=z;  pts[4]=p;

  x = 5.0;  y=-4.0;  error[5]=0; z= -(a[0]*x + a[1]*y - error[5])/a[2] ;
  p[0] = x; p[1]=y; p[2]=z;  pts[5]=p;

  x = 3.0;  y=-2.0;  error[6]=-0.004; z= -(a[0]*x + a[1]*y - error[6])/a[2] ;
  p[0] = x; p[1]=y; p[2]=z;  pts[6]=p;

  //
  //  The first set of tests are for the constructor, and parameter access methods.
  //
  vbl_test_begin( "ctor 1" );
  rrel_estimation_problem * lr1 = new rrel_orthogonal_regression( pts );
  vbl_test_perform( lr1 != 0 );

  
  vbl_test_begin( "num_points_to_instantiate (1)" );
  vbl_test_perform( lr1->num_samples_to_instantiate() == 2 );
  vbl_test_begin( "num_points_to_instantiate (3)" );
  vbl_test_perform( (int)lr1->num_samples() == num_pts );
  vbl_test_begin( "dtor (1)" );
  delete lr1;
  vbl_test_perform( true );
 
 
  //
  //  Test the residuals function.
  //
  vcl_vector<double> residuals;
  lr1 = new rrel_orthogonal_regression( pts );

  vbl_test_begin( "residuals" );
  lr1->compute_residuals( true_params, residuals );
  bool ok = (int(residuals.size()) == num_pts);
  for ( unsigned int i=0; i<residuals.size() && ok; ++ i )  ok = close( residuals[i], error[i] );
  vbl_test_perform( ok );

  //
  //  Test the weighted least squares function.
  //
  vcl_vector<double> wgts( num_pts );
  vnl_matrix<double> cofact;

  // Make weights so that the estimation is singular.
  wgts[0] = 0;   wgts[1] = 1;   wgts[2] = 0;    wgts[3] = 0;  
  wgts[4] = 0;   wgts[5] = 0;   wgts[6] = 0;
  vbl_test_begin( "weighted_least_squares_fit (singular)" );
  vbl_test_perform( !lr1->weighted_least_squares_fit( par, cofact, &wgts ) );

  // Ok.  This one should work.
  ok = lr1->weighted_least_squares_fit( par, cofact );
  vnl_double_3 diff( par - true_params / true_params.two_norm() );
  vbl_test_begin( "weighted_least_squares_fit (ok) ");
  double err = diff.two_norm (); // standardized error
  //  vcl_cout << "estimated params: " << par
  //           << ";  true params: " << true_params << vcl_endl
  //           << "cofactor matrix: \n" << cofact 
  //           << " error : " << err << vcl_endl;
  vbl_test_perform( ok && err <1e-2 ); 
 
  delete lr1;
  vbl_test_summary();
  return 0;
}
