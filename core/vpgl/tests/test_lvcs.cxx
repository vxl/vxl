#include <iostream>
#include "testlib/testlib_test.h"
#include "vpgl/vpgl_lvcs.h"


static void
test_lvcs()
{
  // origin in WGS84 & UTM
  double orig_lat = 38.982859, orig_lon = -117.057278, orig_elev = 1670;
  double orig_easting = 495039.001, orig_northing = 4314875.991;
  int orig_utm_zone = 11;
  bool orig_south_flag = 0;

  // results
  double x, y, z;
  int utm_zone;


  // ----- WGS84 lvcs -----
  std::cout << "\nTest WGS84 LVCS\n";
  vpgl_lvcs lvcs_wgs84(orig_lat, orig_lon, orig_elev, vpgl_lvcs::wgs84,
                       vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  // origin
  std::cout << "origin\n";
  lvcs_wgs84.get_origin(y, x, z);

  TEST_NEAR("longitude", x, orig_lon, 1e-6);
  TEST_NEAR("latitude", y, orig_lat, 1e-6);
  TEST_NEAR("elevation", z, orig_elev, 1e-3);

  // local origin as (0,0,0)
  std::cout << "global_to_local(origin, WGS84)\n";
  lvcs_wgs84.global_to_local(orig_lon, orig_lat, orig_elev, vpgl_lvcs::wgs84,
                             x, y, z);

  TEST_NEAR("local_x", x, 0.0, 1e-3);
  TEST_NEAR("local_y", y, 0.0, 1e-3);
  TEST_NEAR("local_z", z, 0.0, 1e-3);

  // local origin -> WGS84
  std::cout << "global_to_local(origin, WGS84)\n";
  lvcs_wgs84.local_to_global(0.0, 0.0, 0.0,
                             vpgl_lvcs::wgs84, x, y, z);

  TEST_NEAR("longitude", x, orig_lon, 1e-6);
  TEST_NEAR("latitude", y, orig_lat, 1e-6);
  TEST_NEAR("elevation", z, orig_elev, 1e-3);


  // ----- UTM lvcs -----
  std::cout << "\nTest UTM LVCS\n";
  vpgl_lvcs lvcs_utm(orig_lat, orig_lon, orig_elev, vpgl_lvcs::utm,
                     vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  // origin in UTM
  std::cout << "origin\n";
  lvcs_utm.get_utm_origin(x, y, z, utm_zone);

  TEST_NEAR("easting", x, orig_easting, 1e-3);
  TEST_NEAR("northing", y, orig_northing, 1e-3);
  TEST_NEAR("elevation", z, orig_elev, 1e-3);
  TEST("utm_zone", utm_zone, orig_utm_zone);

  // origin in WGS84
  std::cout << "WGS84 origin\n";
  lvcs_utm.get_origin(y, x, z);

  TEST_NEAR("longitude", x, orig_lon, 1e-6);
  TEST_NEAR("latitude", y, orig_lat, 1e-6);
  TEST_NEAR("elevation", z, orig_elev, 1e-3);

  // local origin as (0,0,0)
  std::cout << "global_to_local(origin, UTM)\n";
  lvcs_utm.global_to_local(orig_easting, orig_northing, orig_elev, vpgl_lvcs::utm,
                           x, y, z);

  TEST_NEAR("local_x", x, 0.0, 1e-3);
  TEST_NEAR("local_y", y, 0.0, 1e-3);
  TEST_NEAR("local_z", z, 0.0, 1e-3);

  // local origin -> UTM
  std::cout << "local_to_global(origin, UTM)\n";
  lvcs_utm.local_to_global(0.0, 0.0, 0.0,
                           vpgl_lvcs::utm, x, y, z);

  TEST_NEAR("easting", x, orig_easting, 1e-3);
  TEST_NEAR("northing", y, orig_northing, 1e-3);
  TEST_NEAR("elevation", z, orig_elev, 1e-3);

  // local origin -> WGS84
  std::cout << "local_to_global(origin, WGS84)l\n";
  lvcs_utm.local_to_global(0.0, 0.0, 0.0,
                           vpgl_lvcs::wgs84, x, y, z);

  TEST_NEAR("longitude", x, orig_lon, 1e-6);
  TEST_NEAR("latitude", y, orig_lat, 1e-6);
  TEST_NEAR("elevation", z, orig_elev, 1e-3);

}

TESTMAIN(test_lvcs);
