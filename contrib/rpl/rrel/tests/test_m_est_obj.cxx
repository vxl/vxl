// This is rpl/rrel/tests/test_m_est_obj.cxx
#include <vcl_vector.h>
#include <testlib/testlib_test.h>
#include <vnl/vnl_math.h>

#include <rrel/rrel_tukey_obj.h>
#include <rrel/rrel_cauchy_obj.h>
#include <rrel/rrel_trunc_quad_obj.h>

bool close(double,double);

MAIN( test_m_est_obj )
{
  START( "M-estimator obj fcn." );
  const double c=4.0;
  rrel_m_est_obj * m_est = new rrel_tukey_obj( c);
  const double sigma=2.5;

  //
  // Test the functions specific to the Beaton-Tukey biweight rho function.
  //
  testlib_test_begin( "tukey rho 1" );
  testlib_test_perform( close(m_est->rho(-c-0.001), 1.0) );
  testlib_test_begin( "tukey rho 2" );
  testlib_test_perform( close(m_est->rho(c+0.001), 1.0) );
  testlib_test_begin( "tukey rho 3" );
  testlib_test_perform( close(m_est->rho(0), 0.0) );
  testlib_test_begin( "tukey rho 4" );
  testlib_test_perform( close(m_est->rho(0.5*c), 0.578125) );

  testlib_test_begin( "tukey wgt 1" );
  testlib_test_perform( close(m_est->wgt(-c-0.001), 0) );
  testlib_test_begin( "tukey wgt 2" );
  testlib_test_perform( close(m_est->wgt(c+0.001), 0) );
  testlib_test_begin( "tukey wgt 3" );
  testlib_test_perform( close(m_est->wgt(0), 1.0) );
  testlib_test_begin( "tukey wgt 4" );
  testlib_test_perform( close(m_est->wgt(-0.25*c), 0.878906) );
  testlib_test_begin( "tukey wgt 5" );
  testlib_test_perform( close(m_est->wgt(3.0,sigma), 0.132496) );


  //
  //  Test the MEstimator functions
  //
  vcl_vector<double> res;
  res.push_back(1.0);
  res.push_back(2.1);
  res.push_back(-3.0);
  res.push_back(0.5);
  res.push_back(10.5);
  res.push_back(-1.5);
  int num_res = res.size();
  double hand_obj =
    m_est->rho( res[0] / sigma ) + m_est->rho( res[1] / sigma ) +
    m_est->rho( res[2] / sigma ) + m_est->rho( res[3] / sigma ) +
    m_est->rho( res[4] / sigma ) + m_est->rho( res[5] / sigma );
  testlib_test_begin( "Objective function" );
  double obj = m_est->fcn( res.begin(), res.end(), sigma, 0 );
  testlib_test_perform( close(obj, hand_obj) );

  vcl_vector<double> weights(num_res);
  testlib_test_begin( "Computing all weights --- single scale" );
  m_est->wgt( res.begin(), res.end(), sigma, weights.begin() );
  testlib_test_perform(
    close( m_est->wgt( res[0],sigma ), weights[0] ) &&
    close( m_est->wgt( res[1],sigma ), weights[1] ) &&
    close( m_est->wgt( res[2],sigma ), weights[2] ) &&
    close( m_est->wgt( res[3],sigma ), weights[3] ) &&
    close( m_est->wgt( res[4],sigma ), weights[4] ) &&
    close( m_est->wgt( res[5],sigma ), weights[5] ) );

  testlib_test_begin( "Computing all weights --- individual scales" );
  vcl_vector<double> scales(num_res);
  scales[0] = 1.0;
  scales[1] = 2.0;
  scales[2] = 1.0;
  scales[3] = 1.5;
  scales[4] = 2.5;
  scales[5] = 40.0;
  m_est->wgt( res.begin(), res.end(), scales.begin(), weights.begin() );
  testlib_test_perform(
    close( m_est->wgt( res[0] , scales[0] ), weights[0] ) &&
    close( m_est->wgt( res[1] , scales[1] ), weights[1] ) &&
    close( m_est->wgt( res[2] , scales[2] ), weights[2] ) &&
    close( m_est->wgt( res[3] , scales[3] ), weights[3] ) &&
    close( m_est->wgt( res[4] , scales[4] ), weights[4] ) &&
    close( m_est->wgt( res[5] , scales[5] ), weights[5] ) );

  //
  // Test the functions specific to the Cauchy rho function
  //
  double cauchy_cnst = 2;
  rrel_m_est_obj* m_est2 = new rrel_cauchy_obj( cauchy_cnst );
  testlib_test_begin( "cauchy rho 1" );
  testlib_test_perform( close(m_est2->rho(0), 0.0) );
  testlib_test_begin( "cauchy rho 2" );
  testlib_test_perform( close(m_est2->rho(0.5), 0.0303123 ) );

  testlib_test_begin( "cauchy wgt 1" );
  testlib_test_perform( close(m_est2->wgt(0), 1) );
  testlib_test_begin( "cauchy wgt 2" );
  testlib_test_perform( close(m_est2->wgt(0.5), 0.9411764) );
  testlib_test_begin( "cauchy wgt 3" );
  testlib_test_perform( close(m_est2->wgt(1.0,2.0), 0.9411764/(2*2)) );

  //
  // Test the functions specific to the truncated quadratic rho function
  //
  double trunc_quad_cnst = 2;
  rrel_m_est_obj* m_est3 = new rrel_trunc_quad_obj( trunc_quad_cnst );
  testlib_test_begin( "trunc_quad rho 1" );
  testlib_test_perform( close(m_est3->rho(0), 0.0) );
  testlib_test_begin( "trunc_quad rho 2" );
  testlib_test_perform( close(m_est3->rho(0.5), 0.25 ) );
  testlib_test_begin( "trunc_quad rho 3" );
  testlib_test_perform( close(m_est3->rho(3.0), vnl_math_sqr( trunc_quad_cnst ) ) );

  testlib_test_begin( "trunc_quad wgt 1" );
  testlib_test_perform( close(m_est3->wgt(0), 1) );
  testlib_test_begin( "trunc_quad wgt 2" );
  testlib_test_perform( close(m_est3->wgt(0.5), 1.0) );
  testlib_test_begin( "trunc_quad wgt 3" );
  testlib_test_perform( close(m_est3->wgt(1.0,2.0), 1.0/(2.0*2.0)) );

  delete m_est;
  delete m_est2;
  delete m_est3;

  SUMMARY();
}
