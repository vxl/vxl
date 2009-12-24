//:
// \file
#include <testlib/testlib_test.h>
#include <vpgl/bgeo/bgeo_sun_pos.h>
#include <vcl_cmath.h>

//: Test the sun_pos class
static void test_sun_pos()
{
  unsigned year = 2002;
  unsigned month = 5;
  unsigned day = 25;
  unsigned hours = 6;
  unsigned minutes = 13;
  unsigned seconds = 50;
  double lon = 69.922135;
  double lat = 33.334889;
  double sun_azimuth, sun_elevation;
  bgeo_sun_pos(year, month, day, hours, minutes, seconds, lon, lat, sun_azimuth, sun_elevation);
  double er = vcl_abs(sun_azimuth-127.482) + vcl_abs(sun_elevation-71.266);
  TEST_NEAR("sun_angles_test1", er, 0, 0.01);
  year = 2003;
  month = 7;
  day = 11;
  hours = 1;
  minutes = 8;
  seconds = 56;
  lon = 139.27827188;
  lat = 35.76930165;
  bgeo_sun_pos(year, month, day, hours, minutes, seconds, lon, lat, sun_azimuth, sun_elevation);

  er = vcl_abs(sun_azimuth-115.428) + vcl_abs(sun_elevation-64.495);
  TEST_NEAR("sun_angles_test2", er, 0, 0.01);

  lon = -71.39600021;
  lat = 41.74160407;
  year = 2009;
  month = 3;
  day = 13;
  hours = 15;
  minutes = 44;
  seconds = 42;
  bgeo_sun_pos(year, month, day, hours, minutes, seconds, lon, lat, sun_azimuth, sun_elevation);
  er = vcl_abs(sun_azimuth-155.745) + vcl_abs(sun_elevation-42.766);
  TEST_NEAR("sun_angles_test3", er, 0, 0.01);
}
TESTMAIN( test_sun_pos );
