#include <vcl_cmath.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>

#include <testlib/testlib_test.h>

#include <rrel/rrel_linear_regression.h>

bool close(double,double);

static void test_linear_reg()
{
  vnl_vector<double> true_params(3, 10.0,0.02,-0.1);
  vnl_vector<double> a(true_params);

  const unsigned int num_pts=7;

  //  Build LinearRegression objects exercising both constructors and
  //  the two different options for the first constructor.
  vcl_vector< vnl_vector<double> > pts( num_pts );
  vcl_vector< vnl_vector<double> > ind_vars( num_pts );
  vcl_vector<double> rand_vars( num_pts );
  vcl_vector<double> error( num_pts );

  double x = 1.0, y=-0.5; error[0]=-0.001;
  double z= a[0] + a[1]*x + a[2]*y + error[0]; rand_vars[0] = z;
  pts[0] = vnl_vector<double>(3, x,y,z);
  ind_vars[0] = vnl_vector<double>(3, 1.0,x,y);

  x = 2.0;  y=4.0;  error[1]=0;
  z = a[0] + a[1]*x + a[2]*y + error[1]; rand_vars[1] = z;
  pts[1] = vnl_vector<double>(3, x,y,z);
  ind_vars[1] = vnl_vector<double>(3, 1.0,x,y);

  x = 3.0;  y=1.0;  error[2]=0;
  z = a[0] + a[1]*x + a[2]*y + error[2]; rand_vars[2] = z;
  pts[2] = vnl_vector<double>(3, x,y,z);
  ind_vars[2] = vnl_vector<double>(3, 1.0,x,y);

  x = -2.0;  y=3.0; error[3]=-0.0025;
  z = a[0] + a[1]*x + a[2]*y + error[3]; rand_vars[3] = z;
  pts[3] = vnl_vector<double>(3, x,y,z);
  ind_vars[3] = vnl_vector<double>(3, 1.0,x,y);

  x = 2.0;  y=4.0;  error[4]=0.007;
  z = a[0] + a[1]*x + a[2]*y + error[4]; rand_vars[4] = z;
  pts[4] = vnl_vector<double>(3, x,y,z);
  ind_vars[4] = vnl_vector<double>(3, 1.0,x,y);

  x = 5.0;  y=-4.0;  error[5]=0;
  z = a[0] + a[1]*x + a[2]*y + error[5]; rand_vars[5] = z;
  pts[5] = vnl_vector<double>(3, x,y,z);
  ind_vars[5] = vnl_vector<double>(3, 1.0,x,y);

  x = 3.0;  y=-2.0;  error[6]=-0.004;
  z = a[0] + a[1]*x + a[2]*y + error[6]; rand_vars[6] = z;
  pts[6] = vnl_vector<double>(3, x,y,z);
  ind_vars[6] = vnl_vector<double>(3, 1.0,x,y);

  //
  //  The first set of tests are for the constructor, and parameter access methods.
  //
  testlib_test_begin( "ctor 1" );
  rrel_linear_regression * lr1 = new rrel_linear_regression( pts, true );
  testlib_test_perform( lr1 != 0 );
#if 0
  vcl_cout << "\nPoints with intercept...\n";
  lr1->print_points();
#endif

  testlib_test_begin( "ctor 2" );
  rrel_linear_regression * lr2 = new rrel_linear_regression( pts, false );
  testlib_test_perform( lr2 != 0 );
#if 0
  vcl_cout << "\nPoints without intercept...\n";
  lr2->print_points();
#endif

  testlib_test_begin( "num_samples_to_instantiate (1)" );
  testlib_test_perform( lr1->num_samples_to_instantiate() == 3 );
  testlib_test_begin( "num_samples_to_instantiate (2)" );
  testlib_test_perform( lr2->num_samples_to_instantiate() == 2 );
  testlib_test_begin( "num_samples_to_instantiate (3)" );
  testlib_test_perform( lr1->num_samples() == num_pts );
  testlib_test_begin( "num_data_points" );
  testlib_test_perform( lr2->num_samples() == num_pts );
  testlib_test_begin( "dtor (1)" );
  delete lr1;
  testlib_test_perform( true );
  testlib_test_begin( "dtor (2)" );
  delete lr2;
  testlib_test_perform( true );
  testlib_test_begin( "dtor (3)" );
  rrel_linear_regression * lr3 = new rrel_linear_regression( ind_vars, rand_vars );
  delete lr3;
  testlib_test_perform( true );
  lr3 = new rrel_linear_regression( ind_vars, rand_vars );


  //
  //  The second set of tests uses just lr3 and tests FitFromMinimalSample
  //
  vcl_vector<int> point_indices(3);
  vnl_vector<double> par;

  // should return false because 1&4 have same loc
  point_indices[0] = 1;  point_indices[1] = 2;   point_indices[2] = 4;
  testlib_test_begin( "fit_from_minimal_sample (1) " );
  testlib_test_perform( !lr3->fit_from_minimal_set(point_indices,par) );

  // this one should work
  point_indices[2] = 5;
  testlib_test_begin( "fit_from_minimal_sample (2) " );
  testlib_test_perform( lr3->fit_from_minimal_set(point_indices,par) &&
                        close( (par - true_params).magnitude(), 0 ) );

  //
  //  Test the residuals function.
  //
  vcl_vector<double> residuals( num_pts );
  testlib_test_begin( "residuals" );
  lr3->compute_residuals( par, residuals );
  bool ok = true;
  for ( unsigned int i=0; i<residuals.size() && ok; ++ i )
    ok = close( residuals[i], error[i] );
  testlib_test_perform( ok );

  //
  //  Test the weighted least squares function.
  //
  vnl_matrix<double> cofact;
  vcl_vector<double> wgts( num_pts );

  // Make weights so that the estimation is singular.
  wgts[0] = 0;   wgts[1] = 1;   wgts[2] = 2;    wgts[3] = 0;
  wgts[4] = 1;   wgts[5] = 0;   wgts[6] = 0;
  testlib_test_begin( "weighted_least_squares_fit (singular)" );
  testlib_test_perform( !lr3->weighted_least_squares_fit( par, cofact, &wgts ) );

  // Ok.  This one should work.
  ok = lr3->weighted_least_squares_fit( par, cofact );
  vnl_vector<double> diff( par - true_params );
  double scale = 0.003;  // rough hand guess
  vnl_svd<double> svd_cof( cofact*scale*scale );
  testlib_test_begin( "weighted_least_squares_fit (ok) ");
  double err = vcl_sqrt(dot_product( diff * svd_cof.inverse(), diff )); // standardized error
#if 0
  vcl_cout << "estimated params: " << par
           << ";  true params: " << true_params << vcl_endl
           << "cofactor matrix:\n" << cofact
           << " error : " << err << vcl_endl;
#endif
  testlib_test_perform( ok && err <2.5 );

  delete lr3;
}

TESTMAIN(test_linear_reg);
