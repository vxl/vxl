//:
// \file
#include <testlib/testlib_test.h>
#include <brad/brad_atmospheric_parameters.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>

//: Test the atmospheric_parameters class
static void test_atmospheric_parameters()
{
   // test i/o functions
   brad_atmospheric_parameters atm;
   atm.airlight_ = 120.5;
   atm.skylight_ = 28.0;
   atm.optical_depth_ = 0.20;

   vcl_stringstream ss;
   ss << atm;

   brad_atmospheric_parameters atm2;
   ss >> atm2;

   double tol = 1e-5;
   TEST_NEAR("i/o test: airlight", atm.airlight_,atm2.airlight_,tol);
   TEST_NEAR("i/o test: skylight", atm.skylight_, atm2.skylight_, tol);
   TEST_NEAR("i/o test: optical_depth",atm.optical_depth_, atm2.optical_depth_, tol);
}

TESTMAIN( test_atmospheric_parameters );
