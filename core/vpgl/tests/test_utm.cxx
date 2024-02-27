#include <iostream>
#include "testlib/testlib_test.h"
#include "vpgl/vpgl_utm.h"


void
test_utm_convert(double lat, double lon, double easting, double northing,
                 int utm_zone, bool south_flag,
                 int force_utm_zone=-1, int force_south_flag=-1,
                 double lat_expected=-9999, double lon_expected=-9999)
{
  // report
  std::cout << "\n"
            << "(lat, lon) = (" << lat << ", " << lon << ")\n"
            << "(easting, northing, utm_zone, south_flag) = ("
            << easting << ", " << northing << ", "
            << utm_zone << ", " << south_flag << ")\n"
            << "force_utm_zone = " << force_utm_zone << "\n"
            << "force_south_flag = " << force_south_flag << "\n\n";

  // UTM conversion object
  vpgl_utm u;


  // UTM result tolerance in meters
  double utm_tol = 5e-3; // 0.5 cm

  // WGS84 to UTM
  double easting_result, northing_result;
  int utm_zone_result;
  bool south_flag_result;
  u.transform(lat, lon, easting_result, northing_result,
              utm_zone_result, south_flag_result,
              force_utm_zone, force_south_flag);

  std::cout << "WGS84 -> UTM\nUTM result ("
            << easting_result << ", " << northing_result << ", "
            << utm_zone_result << ", " << south_flag_result << ")\n";
  TEST_NEAR("easting", easting_result, easting, utm_tol);
  TEST_NEAR("northing", northing_result, northing, utm_tol);
  TEST("utm_zone", utm_zone_result, utm_zone);
  TEST("south_flag", south_flag_result, south_flag);


  // WGS84 result tolerance in degrees
  double wgs84_tol = 1e-6;

  // UTM to WGS84
  double lat_result, lon_result;
  u.transform(utm_zone, easting, northing, lat_result, lon_result, south_flag);

  lat_expected = (lat_expected > -9999) ? lat_expected : lat;
  lon_expected = (lon_expected > -9999) ? lon_expected : lon;

  std::cout << "UTM -> WGS84\n"
            << "(lat, lon) expected = ("
            << lat_expected << ", " << lon_expected << ")\n"
            << "(lat, lon) result =   ("
            << lat_result << ", " << lon_result << ")\n";
  TEST_NEAR("latitude", lat_result, lat_expected, wgs84_tol);
  TEST_NEAR("longitude", lon_result, lon_expected, wgs84_tol);

}


void
test_utm2utm(int utm_zone_a, bool south_flag_a,
             double easting_a, double northing_a,
             int utm_zone_b, bool south_flag_b,
             double easting_b, double northing_b)
{
  // report
  std::cout << "\n"
            << "UTM->UTM (easting, northing, utm_zone, south_flag)\n"
            << "A = (" << easting_a << ", " << northing_a << ", "
                   << utm_zone_a << ", " << south_flag_a << ")\n"
            << "B = (" << easting_b << ", " << northing_b << ", "
                   << utm_zone_b << ", " << south_flag_b << ")\n";

  // UTM conversion object
  vpgl_utm u;

  // UTM result tolerance in meters
  double utm_tol = 5e-3;  // 0.5 cm

  // results
  double easting_result, northing_result;

  // A->B
  u.utm2utm(utm_zone_a, south_flag_a, easting_a, northing_a,
            utm_zone_b, south_flag_b, easting_result, northing_result);

  std::cout << "A -> B\n";
  TEST_NEAR("easting", easting_result, easting_b, utm_tol);
  TEST_NEAR("northing", northing_result, northing_b, utm_tol);

  // B->A
  u.utm2utm(utm_zone_b, south_flag_b, easting_b, northing_b,
            utm_zone_a, south_flag_a, easting_result, northing_result);

  std::cout << "B -> A\n";
  TEST_NEAR("easting", easting_result, easting_a, utm_tol);
  TEST_NEAR("northing", northing_result, northing_a, utm_tol);

}

static void
test_utm()
{
  // expected conversions obtained from https://www.ngs.noaa.gov/NCAT/
  test_utm_convert(39.224086,  -98.542151, 539520.675, 4341743.979, 14, 0);
  test_utm_convert(37.905526, -119.987431, 237345.970, 4199541.989, 11, 0);
  test_utm_convert(37.904211, -120.001263, 763652.971, 4199427.978, 10, 0);
  test_utm_convert(38.982859, -117.057278, 495039.001, 4314875.991, 11, 0);

  // antimeridian
  test_utm_convert(71.1,  179.9, 604806.512, 7891059.819, 60, 0);
  test_utm_convert(71.1, -180.1, 604806.512, 7891059.819, 60, 0, -1, -1, 71.1, 179.9);

  test_utm_convert(71.1,  179.9, 387970.849, 7891417.933, 1, 0, 1, 0);
  test_utm_convert(71.1, -180.1, 387970.849, 7891417.933, 1, 0, 1, 0, 71.1, 179.9);

  test_utm_convert(71.1, -179.9, 395193.488, 7891059.819, 1, 0);
  test_utm_convert(71.1,  180.1, 395193.488, 7891059.819, 1, 0, -1, -1, 71.1, -179.9);

  test_utm_convert(71.1, -179.9, 612029.151, 7891417.933, 60, 0, 60, 0);
  test_utm_convert(71.1,  180.1, 612029.151, 7891417.933, 60, 0, 60, 0, 71.1, -179.9);


  // WGS84 -> UTM with forced utm_zone & south_flag
  // The WGS84 point (lat = 0, lon = 72) is both on the equator and on the
  // border between UTM zones 18 & 19.  Determine the UTM coordinate
  // with various forcing selections.
  double lat = 0.001, lon = -72.0001;
  test_utm_convert(lat, lon, 833967.414, 110.683, 18, 0);  // default 18-north
  test_utm_convert(lat, lon, 166010.300, 110.683, 19, 0, 19);  // force 19-north
  test_utm_convert(lat, lon, 833967.414, 10e6 + 110.683, 18, 1, -1, 1);  // force 18-south
  test_utm_convert(lat, lon, 166010.300, 10e6 + 110.683, 19, 1, 19, 1);  // force 19-south

  // UTM to UTM
  test_utm2utm(18, 0, 833967.414, 110.683,  18, 0, 833967.414, 110.683); // default 18-north
  test_utm2utm(18, 0, 833967.414, 110.683,  19, 0, 166010.300, 110.683);  // force 19-north
  test_utm2utm(18, 0, 833967.414, 110.683,  18, 1, 833967.414, 10e6 + 110.683);  // force 19-north
  test_utm2utm(18, 0, 833967.414, 110.683,  19, 1, 166010.300, 10e6 + 110.683);  // force 19-north

}

TESTMAIN(test_utm);
