// This is rpl/rrel/tests/test_ransac_obj.cxx
#include <vnl/vnl_fwd.h> // Leave this hack here to avoid a MSVC internal compiler error.
#include <vcl_vector.h>
#include <testlib/testlib_test.h>

#include <rrel/rrel_ransac_obj.h>

inline double rho( rrel_objective* obj, double r )
{
  vcl_vector<double> res;
  res.push_back( r );
  return obj->fcn( res.begin(), res.end(), 1.0, 0 );
}

static void test_ransac_obj()
{
  //
  // Test the functions specific to the RANSAC rho function
  //
  double ransac_cnst = 2.5;

  rrel_objective* obj = new rrel_ransac_obj( ransac_cnst );
  TEST_NEAR("ransac rho 1", rho(obj,0), 0.0, 1e-6);
  TEST_NEAR("ransac rho 2", rho(obj,0.5), 0.0 , 1e-6);
  TEST_NEAR("ransac rho 3", rho(obj,3.0), 1.0 , 1e-6);
  TEST_NEAR("ransac rho 4", rho(obj,-3.0), 1.0 , 1e-6);
  TEST_NEAR("ransac rho 5", rho(obj,-1.0), 0.0 , 1e-6);

  delete obj;
}

TESTMAIN(test_ransac_obj);
