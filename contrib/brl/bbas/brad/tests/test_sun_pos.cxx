//:
// \file
#include <iostream>
#include <cmath>
#include <testlib/testlib_test.h>
#include <brad/brad_sun_pos.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Test the sun_pos class
static void test_sun_pos()
{
  // Note: Ground truth Earth-Sun distances computed using: http://ssd.jpl.nasa.gov/horizons.cgi
  unsigned year = 2002;
  unsigned month = 5;
  unsigned day = 25;
  unsigned hours = 6;
  unsigned minutes = 13;
  unsigned seconds = 50;
  double lon = 69.922135;
  double lat = 33.334889;

  double sun_azimuth, sun_elevation;
  brad_sun_pos(year, month, day, hours, minutes, seconds, lon, lat, sun_azimuth, sun_elevation);
  double er = std::abs(sun_azimuth-127.482) + std::abs(sun_elevation-71.266);
  TEST_NEAR("sun_angles_test1", er, 0, 0.01);

  double dist = brad_sun_distance(year, month, day, hours, minutes, seconds);
  double dist_er = std::abs(dist - 1.012804);
  TEST_NEAR("sun_distance_test1", dist_er, 0, 1e-4);

  year = 2003;
  month = 7;
  day = 11;
  hours = 1;
  minutes = 8;
  seconds = 56;
  lon = 139.27827188;
  lat = 35.76930165;
  brad_sun_pos(year, month, day, hours, minutes, seconds, lon, lat, sun_azimuth, sun_elevation);

  er = std::abs(sun_azimuth-115.428) + std::abs(sun_elevation-64.495);
  TEST_NEAR("sun_angles_test2", er, 0, 0.01);

  dist = brad_sun_distance(year, month, day, hours, minutes, seconds);
  dist_er = std::abs(dist - 1.016613);
  TEST_NEAR("sun_distance_test2", dist_er, 0, 1e-4);

  lon = -71.39600021;
  lat = 41.74160407;
  year = 2009;
  month = 3;
  day = 13;
  hours = 15;
  minutes = 44;
  seconds = 42;

  brad_sun_pos(year, month, day, hours, minutes, seconds, lon, lat, sun_azimuth, sun_elevation);
  er = std::abs(sun_azimuth-155.745) + std::abs(sun_elevation-42.766);
  TEST_NEAR("sun_angles_test3", er, 0, 0.01);

  dist = brad_sun_distance(year, month, day, hours, minutes, seconds);
  dist_er = std::abs(dist - 0.994044);
  TEST_NEAR("sun_distance_test3", dist_er, 0, 1e-4);

  year = 2004;
  month = 7;
  day = 15;
  hours = 7;
  minutes = 47;
  seconds = 41;
  lon = 44.29977956;
  lat = 33.14200153;
  brad_sun_pos(year, month, day, hours, minutes, seconds, lon, lat, sun_azimuth, sun_elevation);
  er = std::fabs(sun_azimuth-117.9) + std::fabs(sun_elevation-68.7);
  TEST_NEAR("sun_angles_test4", er, 0, 0.15);

  dist = brad_sun_distance(year, month, day, hours, minutes, seconds);
  dist_er = std::abs(dist - 1.016471);
  TEST_NEAR("sun_distance_test4", dist_er, 0, 1e-4);

  // Test Earth-Sun Distance for a future date
  year = 2050;
  month = 2;
  day = 20;
  hours = 7;
  minutes = 47;
  seconds = 41;
  dist = brad_sun_distance(year, month, day, hours, minutes, seconds);
  dist_er = std::abs(dist - 0.988733426);
  TEST_NEAR("sun_distance_test5", dist_er, 0, 1e-4);

}
TESTMAIN( test_sun_pos );
