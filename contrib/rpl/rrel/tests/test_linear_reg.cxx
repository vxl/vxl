#include <iostream>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>

#include <testlib/testlib_test.h>

#include <rrel/rrel_linear_regression.h>

bool close(double,double);

static void test_linear_reg()
{
  vnl_double_3 true_params(10.0,0.02,-0.1);
  vnl_double_3 a = true_params;

  constexpr unsigned int num_pts = 7;

  //  Build LinearRegression objects exercising both constructors and
  //  the two different options for the first constructor.
  std::vector< vnl_vector<double> > pts( num_pts );
  std::vector< vnl_vector<double> > ind_vars( num_pts );
  std::vector<double> rand_vars( num_pts );
  std::vector<double> error( num_pts );

  double x = 1.0, y=-0.5; error[0]=-0.001;
  double z= a[0] + a[1]*x + a[2]*y + error[0]; rand_vars[0] = z;
  pts[0] = vnl_double_3(x,y,z).as_vector();
  ind_vars[0] = vnl_double_3(1.0,x,y).as_vector();

  x = 2.0;  y=4.0;  error[1]=0;
  z = a[0] + a[1]*x + a[2]*y + error[1]; rand_vars[1] = z;
  pts[1] = vnl_double_3(x,y,z).as_vector();
  ind_vars[1] = vnl_double_3(1.0,x,y).as_vector();

  x = 3.0;  y=1.0;  error[2]=0;
  z = a[0] + a[1]*x + a[2]*y + error[2]; rand_vars[2] = z;
  pts[2] = vnl_double_3(x,y,z).as_vector();
  ind_vars[2] = vnl_double_3(1.0,x,y).as_vector();

  x = -2.0;  y=3.0; error[3]=-0.0025;
  z = a[0] + a[1]*x + a[2]*y + error[3]; rand_vars[3] = z;
  pts[3] = vnl_double_3(x,y,z).as_vector();
  ind_vars[3] = vnl_double_3(1.0,x,y).as_vector();

  x = 2.0;  y=4.0;  error[4]=0.007;
  z = a[0] + a[1]*x + a[2]*y + error[4]; rand_vars[4] = z;
  pts[4] = vnl_double_3(x,y,z).as_vector();
  ind_vars[4] = vnl_double_3(1.0,x,y).as_vector();

  x = 5.0;  y=-4.0;  error[5]=0;
  z = a[0] + a[1]*x + a[2]*y + error[5]; rand_vars[5] = z;
  pts[5] = vnl_double_3(x,y,z).as_vector();
  ind_vars[5] = vnl_double_3(1.0,x,y).as_vector();

  x = 3.0;  y=-2.0;  error[6]=-0.004;
  z = a[0] + a[1]*x + a[2]*y + error[6]; rand_vars[6] = z;
  pts[6] = vnl_double_3(x,y,z).as_vector();
  ind_vars[6] = vnl_double_3(1.0,x,y).as_vector();

  //
  //  The first set of tests are for the constructor, and parameter access methods.
  //
  auto * lr1 = new rrel_linear_regression( pts, true );
  TEST( "ctor 1", lr1 != nullptr, true);
#if 0
  std::cout << "\nPoints with intercept...\n";
  lr1->print_points();
#endif

  auto * lr2 = new rrel_linear_regression( pts, false );
  TEST( "ctor 2", lr2 != nullptr, true);
#if 0
  std::cout << "\nPoints without intercept...\n";
  lr2->print_points();
#endif

  TEST( "num_samples_to_instantiate (1)" , lr1->num_samples_to_instantiate(), 3);
  TEST( "num_samples_to_instantiate (2)" , lr2->num_samples_to_instantiate(), 2);
  TEST( "num_samples_to_instantiate (3)" , lr1->num_samples(), num_pts);
  TEST( "num_data_points" , lr2->num_samples(), num_pts);
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
  std::vector<int> point_indices(3);
  vnl_vector<double> par;

  // should return false because 1&4 have same loc
  point_indices[0] = 1;  point_indices[1] = 2;   point_indices[2] = 4;

  TEST( "fit_from_minimal_sample (1)", !lr3->fit_from_minimal_set(point_indices,par), true);

  // this one should work
  point_indices[2] = 5;
  TEST("fit_from_minimal_sample (2)", lr3->fit_from_minimal_set(point_indices,par) &&
                                      close( (par - true_params).magnitude(), 0 ), true);

  //
  //  Test the residuals function.
  //
  std::vector<double> residuals( num_pts );
  lr3->compute_residuals( par, residuals );
  bool ok = true;
  for ( unsigned int i=0; i<residuals.size() && ok; ++ i )
    ok = close( residuals[i], error[i] );
  TEST("residuals", ok, true);

  //
  //  Test the weighted least squares function.
  //
  vnl_matrix<double> cofact;
  std::vector<double> wgts( num_pts );

  // Make weights so that the estimation is singular.
  wgts[0] = 0;   wgts[1] = 1;   wgts[2] = 2;    wgts[3] = 0;
  wgts[4] = 1;   wgts[5] = 0;   wgts[6] = 0;
  TEST( "weighted_least_squares_fit (singular)", !lr3->weighted_least_squares_fit( par, cofact, &wgts ), true);

  // Ok.  This one should work.
  ok = lr3->weighted_least_squares_fit( par, cofact );
  vnl_vector<double> diff( par - true_params );
  double scale = 0.003;  // rough hand guess
  vnl_svd<double> svd_cof( cofact*scale*scale );
  double err = std::sqrt(dot_product( diff * svd_cof.inverse(), diff )); // standardized error
#if 0
  std::cout << "estimated params: " << par
           << ";  true params: " << true_params << std::endl
           << "cofactor matrix:\n" << cofact
           << " error : " << err << std::endl;
#endif
  TEST( "weighted_least_squares_fit (ok) ", ok && err <2.5, true);

  delete lr3;
}

TESTMAIN(test_linear_reg);
