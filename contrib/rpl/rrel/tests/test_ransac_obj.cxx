// This is rpl/rrel/tests/test_ransac_obj.cxx
#include <vnl/vnl_fwd.h> // Leave this hack here to avoid a MSVC internal compiler error.
#include <vcl_vector.h>
#include <testlib/testlib_test.h>

#include <rrel/rrel_ransac_obj.h>

bool close(double,double);

double rho( rrel_objective* obj, double r )
{
  vcl_vector<double> res;
  res.push_back( r );
  return obj->fcn( res.begin(), res.end(), 1.0, 0 );
}

MAIN( test_ransac_obj )
{
  START( "RANSAC obj fcn." );

  //
  // Test the functions specific to the RANSAC rho function
  //
  double ransac_cnst = 2.5;

  rrel_objective* obj = new rrel_ransac_obj( ransac_cnst );
  testlib_test_begin( "ransac rho 1" );
  testlib_test_perform( close(rho(obj,0), 0.0) );
  testlib_test_begin( "ransac rho 2" );
  testlib_test_perform( close(rho(obj,0.5), 0.0 ) );
  testlib_test_begin( "ransac rho 3" );
  testlib_test_perform( close(rho(obj,3.0), 1.0 ) );
  testlib_test_begin( "ransac rho 4" );
  testlib_test_perform( close(rho(obj,-3.0), 1.0 ) );
  testlib_test_begin( "ransac rho 5" );
  testlib_test_perform( close(rho(obj,-1.0), 0.0 ) );

  delete obj;

  SUMMARY();
}
