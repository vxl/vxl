#include <iostream>
#include <iomanip>
#include "testlib/testlib_test.h"
#include "vpgl/vpgl_lvcs.h"

void
test_lvcs_force(double lat, double lon, double elev,
                double easting, double northing,
                int utm_zone, bool south_flag,
                double meter_tol, double degree_tol)
{
  // report
  std::cout << "\nTest UTM LVCS with zone/hemisphere\n"
            << "(lat, lon) = (" << lat << ", " << lon << ")\n"
            << "(easting, northing, utm_zone, south_flag) = ("
            << easting << ", " << northing << ", "
            << utm_zone << ", " << south_flag << ")\n";

  // results
  double x, y, z;
  int utm_zone_result;
  bool south_flag_result;

  // LVCS
  vpgl_lvcs lvcs(lat, lon, elev, vpgl_lvcs::utm,
                 vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  // force UTM zone
  lvcs.set_utm(utm_zone, south_flag);

  // test UTM zone
  std::cout << "get_utm()\n";
  lvcs.get_utm(utm_zone_result, south_flag_result);
  TEST("utm_zone", utm_zone_result, utm_zone);
  TEST("south_flag", south_flag_result, south_flag);

  // test origin
  std::cout << "get_utm_origin()\n";
  lvcs.get_utm_origin(x, y, z, utm_zone_result, south_flag_result);
  TEST_NEAR("easting", x, easting, meter_tol);
  TEST_NEAR("northing", y, northing, meter_tol);
  TEST_NEAR("elevation", z, elev, meter_tol);
  TEST("utm_zone", utm_zone_result, utm_zone);
  TEST("south_flag", south_flag_result, south_flag);

  // test WGS84 global->local at origin
  std::cout << "global_to_local(origin, WGS84)\n";
  lvcs.global_to_local(lon, lat, elev, vpgl_lvcs::wgs84, x, y, z);
  TEST_NEAR("local_x", x, 0.0, meter_tol);
  TEST_NEAR("local_y", y, 0.0, meter_tol);
  TEST_NEAR("local_z", z, 0.0, meter_tol);

  // test UTM global->local at origin
  std::cout << "global_to_local(origin, UTM)\n";
  lvcs.global_to_local(easting, northing, elev, vpgl_lvcs::utm, x, y, z);
  TEST_NEAR("local_x", x, 0.0, meter_tol);
  TEST_NEAR("local_y", y, 0.0, meter_tol);
  TEST_NEAR("local_z", z, 0.0, meter_tol);

  // test WGS84 local->global at origin
  std::cout << "local_to_global(origin, WGS84)\n";
  lvcs.local_to_global(0.0, 0.0, 0.0, vpgl_lvcs::wgs84, x, y, z);
  TEST_NEAR("longitude", x, lon, degree_tol);
  TEST_NEAR("latitude", y, lat, degree_tol);
  TEST_NEAR("elevation", z, elev, meter_tol);

  // test UTM local->global at origin
  std::cout << "local_to_global(origin, UTM)\n";
  lvcs.local_to_global(0.0, 0.0, 0.0, vpgl_lvcs::utm, x, y, z);
  TEST_NEAR("local_to_global UTM easting", x, easting, meter_tol);
  TEST_NEAR("local_to_global UTM northing", y, northing, meter_tol);
  TEST_NEAR("local_to_global UTM elevation", z, elev, meter_tol);
}


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
  bool south_flag;

  // result tolerance
  double meter_tol = 1e-3;
  double degree_tol = 1e-6;


  // ----- WGS84 lvcs -----
  std::cout << "\nTest WGS84 LVCS\n";
  vpgl_lvcs lvcs_wgs84(orig_lat, orig_lon, orig_elev, vpgl_lvcs::wgs84,
                       vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  // origin
  std::cout << "origin\n";
  lvcs_wgs84.get_origin(y, x, z);

  TEST_NEAR("longitude", x, orig_lon, degree_tol);
  TEST_NEAR("latitude", y, orig_lat, degree_tol);
  TEST_NEAR("elevation", z, orig_elev, meter_tol);

  // local origin as (0,0,0)
  std::cout << "global_to_local(origin, WGS84)\n";
  lvcs_wgs84.global_to_local(orig_lon, orig_lat, orig_elev, vpgl_lvcs::wgs84,
                             x, y, z);

  TEST_NEAR("local_x", x, 0.0, meter_tol);
  TEST_NEAR("local_y", y, 0.0, meter_tol);
  TEST_NEAR("local_z", z, 0.0, meter_tol);

  // local origin -> WGS84
  std::cout << "local_to_global(origin, WGS84)\n";
  lvcs_wgs84.local_to_global(0.0, 0.0, 0.0,
                             vpgl_lvcs::wgs84, x, y, z);

  TEST_NEAR("longitude", x, orig_lon, degree_tol);
  TEST_NEAR("latitude", y, orig_lat, degree_tol);
  TEST_NEAR("elevation", z, orig_elev, meter_tol);


  // ----- UTM lvcs -----
  std::cout << "\nTest UTM LVCS\n";
  vpgl_lvcs lvcs_utm(orig_lat, orig_lon, orig_elev, vpgl_lvcs::utm,
                     vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  // origin in UTM
  std::cout << "get_utm_origin(x, y, z, zone)\n";
  lvcs_utm.get_utm_origin(x, y, z, utm_zone);

  TEST_NEAR("easting", x, orig_easting, meter_tol);
  TEST_NEAR("northing", y, orig_northing, meter_tol);
  TEST_NEAR("elevation", z, orig_elev, meter_tol);
  TEST("utm_zone", utm_zone, orig_utm_zone);

  // origin in UTM with south_flag
  std::cout << "get_utm_origin(x, y, z, zone, south)\n";
  lvcs_utm.get_utm_origin(x, y, z, utm_zone, south_flag);
  TEST_NEAR("easting", x, orig_easting, meter_tol);
  TEST_NEAR("northing", y, orig_northing, meter_tol);
  TEST_NEAR("elevation", z, orig_elev, meter_tol);
  TEST("utm_zone", utm_zone, orig_utm_zone);
  TEST("south_flag", south_flag, orig_south_flag);

  // origin in WGS84
  std::cout << "WGS84 origin\n";
  lvcs_utm.get_origin(y, x, z);

  TEST_NEAR("longitude", x, orig_lon, degree_tol);
  TEST_NEAR("latitude", y, orig_lat, degree_tol);
  TEST_NEAR("elevation", z, orig_elev, meter_tol);

  // local origin as (0,0,0)
  std::cout << "global_to_local(origin, UTM)\n";
  lvcs_utm.global_to_local(orig_easting, orig_northing, orig_elev, vpgl_lvcs::utm,
                           x, y, z);

  TEST_NEAR("local_x", x, 0.0, meter_tol);
  TEST_NEAR("local_y", y, 0.0, meter_tol);
  TEST_NEAR("local_z", z, 0.0, meter_tol);

  // local origin -> UTM
  std::cout << "local_to_global(origin, UTM)\n";
  lvcs_utm.local_to_global(0.0, 0.0, 0.0,
                           vpgl_lvcs::utm, x, y, z);

  TEST_NEAR("easting", x, orig_easting, meter_tol);
  TEST_NEAR("northing", y, orig_northing, meter_tol);
  TEST_NEAR("elevation", z, orig_elev, meter_tol);

  // local origin -> WGS84
  std::cout << "local_to_global(origin, WGS84)\n";
  lvcs_utm.local_to_global(0.0, 0.0, 0.0,
                           vpgl_lvcs::wgs84, x, y, z);

  TEST_NEAR("longitude", x, orig_lon, degree_tol);
  TEST_NEAR("latitude", y, orig_lat, degree_tol);
  TEST_NEAR("elevation", z, orig_elev, meter_tol);


  // ----- UTM force_utm_zone/force_south_flag -----
  // The WGS84 point (lat = 0, lon = 72) is both on the equator and on the
  // border between UTM zones 18 & 19.  Determine the UTM coordinate
  // with various forcing selections.
  orig_lat = 0.001, orig_lon = -72.0001, orig_elev = 1000;

  // default 18-north
  test_lvcs_force(orig_lat, orig_lon, orig_elev,
                  833967.414, 110.683, 18, 0,
                  meter_tol, degree_tol);
  // force 19-north
  test_lvcs_force(orig_lat, orig_lon, orig_elev,
                  166010.300, 110.683, 19, 0,
                  meter_tol, degree_tol);
  // force 18-south
  test_lvcs_force(orig_lat, orig_lon, orig_elev,
                  833967.414, 10e6 + 110.683, 18, 1,
                  meter_tol, degree_tol);
  // force 19-south
  test_lvcs_force(orig_lat, orig_lon, orig_elev,
                  166010.300, 10e6 + 110.683, 19, 1,
                  meter_tol, degree_tol);

}

TESTMAIN(test_lvcs);
