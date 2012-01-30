//:
// \file
#include <testlib/testlib_test.h>
#include <brad/brad_image_metadata.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>

//: Test the image_metadata class
static void test_image_metadata()
{
   // test i/o functions
   brad_image_metadata md;
   md.sun_azimuth_ = 205;
   md.sun_elevation_ = 45;
   md.view_azimuth_ = 30;
   md.view_elevation_ = 82;
   md.gain_ = 100;
   md.offset_ = 24;
   md.sun_irradiance_ = 1350.5;
   md.number_of_bits_ = 11;

   vcl_stringstream ss;
   ss << md;

   brad_image_metadata md2;
   ss >> md2;

   double tol = 1e-5;
   TEST_NEAR("i/o test: sun_azimuth", md.sun_azimuth_,md2.sun_azimuth_,tol);
   TEST_NEAR("i/o test: sun_elevation", md.sun_elevation_, md2.sun_elevation_, tol);
   TEST_NEAR("i/o test: view_azimuth",md.view_azimuth_, md2.view_azimuth_, tol);
   TEST_NEAR("i/o test: view_elevation",md.view_elevation_, md2.view_elevation_, tol);
   TEST_NEAR("i/o test: gain",md.gain_, md2.gain_, tol);
   TEST_NEAR("i/o test: offset",md.offset_, md2.offset_, tol);
   TEST_NEAR("i/o test: sun_irradiance",md.sun_irradiance_, md2.sun_irradiance_, tol);
   TEST_EQUAL("i/o test: number_of_bits",md.number_of_bits_, md2.number_of_bits_);
}

TESTMAIN( test_image_metadata );
