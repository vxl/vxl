
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vbl/vbl_test.h>
#include <vnl/vnl_math.h>

#include <rrel/rrel_ransac_obj.h>

bool close( double x, double y ) { return vnl_math_abs(x-y) < 1.0e-4; }

double rho( rrel_objective* obj, double r )
{
  vcl_vector<double> res;
  res.push_back( r );
  return obj->fcn( res.begin(), res.end(), 1.0, 0 );
}

int
main()
{
  vbl_test_start( "RANSAC obj fcn." );

  //
  // Test the functions specific to the RANSAC rho function
  //
  double ransac_cnst = 2.5;

  rrel_objective* obj = new rrel_ransac_obj( ransac_cnst );
  vbl_test_begin( "ransac rho 1" );
  vbl_test_perform( close(rho(obj,0), 0.0) );
  vbl_test_begin( "ransac rho 2" );
  vbl_test_perform( close(rho(obj,0.5), 0.0 ) );
  vbl_test_begin( "ransac rho 3" );
  vbl_test_perform( close(rho(obj,3.0), 1.0 ) );
  vbl_test_begin( "ransac rho 4" );
  vbl_test_perform( close(rho(obj,-3.0), 1.0 ) );
  vbl_test_begin( "ransac rho 5" );
  vbl_test_perform( close(rho(obj,-1.0), 0.0 ) );

  vbl_test_summary();
  return 0;
}
