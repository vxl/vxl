#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_matrix.h>

#include <rrel/rrel_tukey_obj.h>
#include <rrel/rrel_linear_regression.h>
#include <rrel/rrel_irls.h>

#include <testlib/testlib_test.h>

#include "similarity_from_matches.h"

double noise( double sigma );

const double conv_tolerance=1.0e-5;

static void
regression_points( const vnl_vector<double>& a,
                   double sigma,
                   vcl_vector< vnl_vector<double> >& pts )
{
  const int num_pts=20;
  pts.resize( num_pts );

  //  Initialize variables.
  double x = 1.0, y=-0.5, z= a[0] + a[1]*x + a[2]*y;
  pts[0]=vnl_double_3(x,y,z).as_vector();

  x = 2.0;  y=4.0; z = a[0] + a[1]*x + a[2]*y;
  pts[1]=vnl_double_3(x,y,z).as_vector();

  x = 3.0;  y=1.0; z = a[0] + a[1]*x + a[2]*y;
  pts[2]=vnl_double_3(x,y,z).as_vector();

  x = -2.0;  y=3.0; z = a[0] + a[1]*x + a[2]*y;
  pts[3]=vnl_double_3(x,y,z).as_vector();

  x = 2.0;  y=4.0;  z = a[0] + a[1]*x + a[2]*y;
  pts[4]=vnl_double_3(x,y,z).as_vector();

  x = 5.0;  y=-4.0; z = a[0] + a[1]*x + a[2]*y;
  pts[5]=vnl_double_3(x,y,z).as_vector();

  x = 3.0;  y=-2.0; z = a[0] + a[1]*x + a[2]*y;
  pts[6]=vnl_double_3(x,y,z).as_vector();

  x = 2.0;  y=-2.0; z = a[0] + a[1]*x + a[2]*y;
  pts[7]=vnl_double_3(x,y,z).as_vector();

  x = 3.0;  y=0.0;  z = a[0] + a[1]*x + a[2]*y;
  pts[8]=vnl_double_3(x,y,z).as_vector();

  x = -1.0; y=-2.0; z = a[0] + a[1]*x + a[2]*y;
  pts[9]=vnl_double_3(x,y,z).as_vector();

  x = 0.0;  y=0.0;  z = a[0] + a[1]*x + a[2]*y;
  pts[10]=vnl_double_3(x,y,z).as_vector();

  x = -1.0; y=2.0;  z = a[0] + a[1]*x + a[2]*y;
  pts[11]=vnl_double_3(x,y,z).as_vector();

  x = 3.5; y=5.0;  z = a[0] + a[1]*x + a[2]*y;
  pts[12]=vnl_double_3(x,y,z).as_vector();

  x = 4.5; y=5.5;  z = a[0] + a[1]*x + a[2]*y;
  pts[13]=vnl_double_3(x,y,z).as_vector();

  x = 3.5; y=6.0;  z = a[0] + a[1]*x + a[2]*y;
  pts[14]=vnl_double_3(x,y,z).as_vector();

  x = -3.5; y=5.0;  z = a[0] + a[1]*x + a[2]*y;
  pts[15]=vnl_double_3(x,y,z).as_vector();

  x = -2.5; y=-4.0;  z = a[0] + a[1]*x + a[2]*y;
  pts[16]=vnl_double_3(x,y,z).as_vector();

  x = 3.5; y=7.0;  z = a[0] + a[1]*x + a[2]*y;
  pts[17]=vnl_double_3(x,y,z).as_vector();

  x = -4.5; y=-4.0;  z = a[0] + a[1]*x + a[2]*y;
  pts[18]=vnl_double_3(x,y,z).as_vector();

  x = 6.5; y=4.0;  z = a[0] + a[1]*x + a[2]*y;
  pts[19]=vnl_double_3(x,y,z).as_vector();

  for ( int i=0; i<num_pts; ++i )
    if ( i%10 == 5 )  //
      pts[i][2] += noise( 20 * sigma);
    else
      pts[i][2] += noise( sigma );
}

inline bool
check( const vnl_vector<double>& correct_params,
       rrel_irls* irls )
{
  vnl_vector<double> res(irls->params());
  double s = irls->scale();
  vnl_matrix<double> covar(irls->cofactor()*s*s);
  vnl_vector<double> err_vector(res - correct_params);

  //  Get standardized error
  double err = vcl_sqrt(dot_product( err_vector*covar, err_vector ));
  bool success = err < 0.5*s;
#ifdef DEBUG
  bool conv = ( irls->did_it_converge( ) );
  vcl_cout << "Finished:\n"
           << "  estimate = " << irls->estimate()
           << ", true fit = " << correct_params << '\n'
           << "  Mahalanobis param error = " << err << '\n'
           << "  " << (success ? "success" : "fail") << '\n'
           << "  scale estimate = " << s  << '\n'
           << "  iterations used = " << irls->iterations_used() << '\n'
           << "  did it converge? " << (conv ? "yes\n" : "no\n");
#endif
 return success;
}


static void test_irls()
{
  //  Set true parameter estimate.
  vnl_double_3 true_params(10.0, 0.02, -0.1);

  //  Create the linear regression problem and an m_estimator objective
  //  function.
  //
  bool use_intercept=true;
  double sigma = 0.25;
  vcl_vector< vnl_vector<double> > pts;
  regression_points( true_params.as_vector(), sigma, pts );
  rrel_linear_regression * lr = new rrel_linear_regression( pts, use_intercept );
  int dof = lr->param_dof();
  rrel_wls_obj * m_est = new rrel_tukey_obj( dof );
  int max_iterations = 50;
  testlib_test_begin( "ctor" );
  rrel_irls * irls = new rrel_irls( max_iterations );
  testlib_test_perform( irls != 0 );

  //  Setting max iteration parameters.
  max_iterations = 50;
  irls->set_max_iterations( max_iterations );

  //  Setting scale estimation parameters.
  int iterations_for_scale = 2;
  bool use_weighted_scale = false;
  testlib_test_begin( "scale parameters for non-weighted scale" );
  irls->set_est_scale( iterations_for_scale, use_weighted_scale );
  testlib_test_perform( true );
  testlib_test_begin( "use convergence test" );
  irls->set_convergence_test( conv_tolerance );
  testlib_test_perform( true );

  testlib_test_begin( "irls with scale estimation" );
  bool success = irls->estimate( lr, m_est ) && check( true_params.as_vector(), irls );
  vcl_cout << "scale:  correct = " << sigma << ", estimated = " << irls->scale() << vcl_endl;
  testlib_test_perform( success );

  irls->reset_params();
  irls->reset_scale();
  use_weighted_scale = true;
  irls->set_est_scale( iterations_for_scale, use_weighted_scale );

  testlib_test_begin( "irls with weighted scale" );
  success = irls->estimate( lr, m_est ) && check( true_params.as_vector(), irls );
  vcl_cout << "scale:  correct = " << sigma << ", estimated = " << irls->scale() << vcl_endl;
  testlib_test_perform( success );
  testlib_test_begin( "did it converge?" );
  testlib_test_perform( irls->converged() );

  irls->reset_params();
  irls->reset_scale();
  irls->initialize_params( true_params.as_vector() );
  testlib_test_begin( "irls with correct initial fit" );
  success = irls->estimate( lr, m_est ) && check( true_params.as_vector(), irls );
  vcl_cout << "scale:  correct = " << sigma << ", estimated = " << irls->scale() << vcl_endl;
  testlib_test_perform( success );

  irls->reset_params();
  irls->initialize_scale( sigma );
  irls->set_no_scale_est();
  irls->set_no_convergence_test();

  testlib_test_begin( "irls with fixed scale" );
  max_iterations=6;
  irls->set_max_iterations( max_iterations );
  success = irls->estimate( lr, m_est ) && check( true_params.as_vector(), irls );
  testlib_test_perform( success );
  testlib_test_begin( "scale unchanged" );
  testlib_test_perform( irls->scale() == sigma );
  testlib_test_begin( "iterations used" );
  testlib_test_perform( irls->iterations_used() == max_iterations );

  //  onto irls from matches
  int trace_level = 0;
  vnl_double_4 params(1.2,0.3,15.0,-4.0);
  vcl_vector< image_point_match > matches;
  sigma = 0.25;
  generate_similarity_matches( params.as_vector(), sigma, matches );
  rrel_estimation_problem* match_prob = new similarity_from_matches( matches );

  rrel_irls irls_m( 20 );
  irls_m.set_trace_level( trace_level );
  irls_m.set_est_scale( 2, true );  // use weighted
  irls_m.set_convergence_test();
  irls_m.initialize_params( params.as_vector() );

  testlib_test_begin( "non-unique matches -- params initialized correctly, weighted scale" );
  testlib_test_perform( irls_m.estimate( match_prob, m_est ) &&
                        check( params.as_vector(), &irls_m ) );
  vcl_cout << "true scale = " << sigma << ", weighted scale = " << irls_m.scale() << vcl_endl;

  irls_m.reset_params();
  irls_m.initialize_params( params.as_vector() );
  irls_m.set_est_scale( 2, false );  // use un-weighted scale
  irls_m.set_convergence_test();
  testlib_test_begin( "non-unique matches -- params initialized correctly, MAD scale" );
  testlib_test_perform( irls_m.estimate( match_prob, m_est ) &&
                        check( params.as_vector(), &irls_m ) );
  vcl_cout << "true scale = " << sigma << ", MAD scale = " << irls_m.scale() << vcl_endl;

  irls_m.reset_params();
  irls_m.initialize_params( params.as_vector() );
  irls_m.set_no_scale_est( );  // use no scale
  irls_m.initialize_scale( sigma );
  irls_m.set_convergence_test();
  testlib_test_begin( "non-unique matches -- params initialized correctly, fixed scale" );
  testlib_test_perform( irls_m.estimate( match_prob, m_est ) &&
                        check( params.as_vector(), &irls_m ) );
  TEST_NEAR("scale unchanged", sigma, irls_m.scale() , 1e-6);

  delete lr;
  delete m_est;
  delete irls;
  delete match_prob;
}

TESTMAIN(test_irls);
