#include <testlib/testlib_test.h>

#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_utm.h>

static void test_lvcs()
{
  // first check utm projection
  // pt 1 near border: 11 S 237346.15 m E 4199542.50 m N   , wgs84 (from google earth):  lat 37.905526(degrees) 37 (degrees)54'19.89"N lon -119.987431(degrees) 119(degrees)59'14.75"W
  //   pt1 from http://www.ngs.noaa.gov/cgi-bin/utm_getut.prl     11 S  237346.006 m E   4199541.877 m N
  // pt 2 near border: 10 S 763653.66 m E 4199428.45 m N   , wgs84 (from google earth):  lat 37.904211(degrees)  37(degrees)54'15.16"N lon -120.001263(degrees) 120(degrees) 0'4.55"W
  // pt 3 mid        : 11 S 495039.78 m E 4314876.95 m N   , wgs84 (from google earth):  lat 38.982859(degrees)  38(degrees)58'58.29"N lon -117.057278(degrees) 117(degrees) 3'26.20"W
  // pt 3 mid from http://www.ngs.noaa.gov/cgi-bin/utm_getut.prl  4314875.917  495039.020   11

  double lat1, lon1, lat2, lon2, lat3, lon3;
  lat1 = 37.905526; lon1 = -119.987431;
  lat2 = 37.904211; lon2 = -120.001263;
  lat3 = 38.982859; lon3 = -117.057278;
  vpgl_utm u;
  double x1,y1; int zone1; double x2,y2; int zone2; double x3,y3; int zone3;
  u.transform(lat1, lon1, x1,y1,zone1);
  TEST("wgs84 to utm, pt1 zone", zone1, 11);
  TEST_NEAR("wgs84 to utm, pt1 x", x1, 237346.15, 1);
  TEST_NEAR("wgs84 to utm, pt1 y", y1, 4199542.5, 1);

  TEST_NEAR("wgs84 to utm (noaa), pt1 x", x1, 237346.006, 1);
  TEST_NEAR("wgs84 to utm (noaa), pt1 y", y1, 4199541.877, 1);

  u.transform(lat2, lon2, x2,y2,zone2);
  TEST("wgs84 to utm, pt2 zone", zone2, 10);
  TEST_NEAR("wgs84 to utm, pt2 x", x2, 763653.66, 1);
  TEST_NEAR("wgs84 to utm, pt2 y", y2, 4199428.45, 1);

  u.transform(lat3, lon3, x3,y3,zone3);
  TEST("wgs84 to utm, pt3 zone", zone3, 11);
  TEST_NEAR("wgs84 to utm, pt3 x", x3, 495039.78, 1);
  TEST_NEAR("wgs84 to utm, pt3 y", y3, 4314876.95, 1);

  TEST_NEAR("wgs84 to utm (noaa), pt3 x", x3, 495039.78, 1);
  TEST_NEAR("wgs84 to utm (noaa), pt3 y", y3, 4314876.95, 1);
}

TESTMAIN(test_lvcs);
