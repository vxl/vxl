#include <iostream>
#include <iomanip>
#include <vector>
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


// test LVCS local->global, offset local input, WGS84 output
void
_test_lvcs_local_to_global_wgs84(
    vpgl_lvcs lvcs,
    double lon, double lat, double elev,
    double xoff, double yoff, double zoff,
    double meter_tol, double degree_tol,
    std::string message="")
{
  // report
  std::cout << "local_to_global(offset ["
            << xoff << ", " << yoff << ", " << zoff << "]) -> WGS84";
  if (!message.empty())
    std::cout << ", " << message;
  std::cout << "\n";

  // local_to_global
  double x, y, z;
  lvcs.local_to_global(xoff, yoff, zoff, vpgl_lvcs::wgs84, x, y, z,
                       vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  std::cout << "produced (lon, lat, elev) = ("
            << x << ", " << y << ", " << z << ")\n";

  // test returned longitude moved in the expected direction
  if (xoff > 0)
    TEST_EQUAL("longitude positive shift", x > lon, true);
  else if (xoff < 0)
    TEST_EQUAL("longitude negative shift", x < lon, true);
  else
    TEST_NEAR("longitude", x, lon, meter_tol);

  // test returned latitude moved in the expected direction
  if (yoff > 0)
    TEST_EQUAL("latitude positive shift", y > lat, true);
  else if (yoff < 0)
    TEST_EQUAL("latitude negative shift", y < lat, true);
  else
    TEST_NEAR("latitude", y, lat, meter_tol);

  // test z
  TEST_NEAR("elevation", z, elev + zoff, meter_tol);
}


// test LVCS local->global, offset local input, UTM output
void
_test_lvcs_local_to_global_utm(
    vpgl_lvcs lvcs,
    double easting, double northing, double elev,
    double xoff, double yoff, double zoff,
    double meter_tol,
    std::string message="")
{
  // report
  std::cout << "local_to_global(offset ["
            << xoff << ", " << yoff << ", " << zoff << "]) -> UTM";
  if (!message.empty())
    std::cout << ", " << message;
  std::cout << "\n";

  // local_to_global
  double x, y, z;
  lvcs.local_to_global(xoff, yoff, zoff, vpgl_lvcs::utm, x, y, z,
                       vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  std::cout << "produced (easting, northing, elev) = ("
            << x << ", " << y << ", " << z << ")\n";

  // test x/y/z
  TEST_NEAR("easting", x, easting + xoff, meter_tol);
  TEST_NEAR("northing", y, northing + yoff, meter_tol);
  TEST_NEAR("elevation", z, elev + zoff, meter_tol);
}


// test LVCS global->local, offset WGS84 input, local output
void
_test_lvcs_global_to_local_wgs84(
    vpgl_lvcs lvcs,
    double lon, double lat, double elev,
    double lon_off, double lat_off, double elev_off,
    double meter_tol,
    std::string message="")
{
  // report
  std::cout << "global_to_local(WGS84 offset ["
            << lon_off << ", " << lat_off << ", " << elev_off << "]";
  if (!message.empty())
    std::cout << ", " << message;
  std::cout << ")\n";

  // global_to_local
  double x, y, z;
  lvcs.global_to_local(lon + lon_off, lat + lat_off, elev + elev_off,
                       vpgl_lvcs::wgs84, x, y, z);
  std::cout << "produced (x, y, z) = ("
            << x << ", " << y << ", " << z << ")\n";

  // test returned x moved in the expected direction
  if (lon_off > 0)
    TEST_EQUAL("local_x positive shift", x > 0, true);
  else if (lon_off < 0)
    TEST_EQUAL("local_x negative shift", x < 0, true);
  else
    TEST_NEAR("local_x", x, 0.0, meter_tol);

  // test returned y moved in the expected direction
  if (lat_off > 0)
    TEST_EQUAL("local_y positive shift", y > 0, true);
  else if (lat_off < 0)
    TEST_EQUAL("local_y negative shift", y < 0, true);
  else
    TEST_NEAR("local_y", y, 0.0, meter_tol);

  // test z
  TEST_NEAR("local_z", z, elev_off, meter_tol);
}


// test LVCS global->local, offset UTM input, local output
void
_test_lvcs_global_to_local_utm(
    vpgl_lvcs lvcs,
    double easting, double northing, double elev,
    double easting_off, double northing_off, double elev_off,
    double meter_tol,
    std::string message="")
{
  // report
  std::cout << "global_to_local(UTM offset [" << easting_off << ", "
            << northing_off << ", " << elev_off << "]";
  if (!message.empty())
    std::cout << ", " << message;
  std::cout << ")\n";

  // global_to_local
  double x, y, z;
  lvcs.global_to_local(easting + easting_off, northing + northing_off,
                       elev + elev_off, vpgl_lvcs::utm, x, y, z);

  // test x/y/z
  TEST_NEAR("local_x", x, easting_off, meter_tol);
  TEST_NEAR("local_y", y, northing_off, meter_tol);
  TEST_NEAR("local_z", z, elev_off, meter_tol);
}


void
_test_lvcs_antimeridian(vpgl_lvcs lvcs,
                        double lon, double lat, double elev,
                        double meter_tol, double degree_tol)
{
  // results
  double x, y, z;
  double easting, northing;
  int utm_zone;
  bool south_flag;

  // meter offsets as (x, y, z)
  std::vector<std::vector<double> > meter_offsets = {
      {0, 0, 0},
      {100, 100, 100},
      {-100, -100, -100},
  };

  // WGS84 offsets as (lon, lat, elev)
  std::vector<std::vector<double> > wgs84_offsets = {
      {0, 0, 0},
      {0.01, 0.01, 100},
      {-0.01, -0.01, -100},
  };

  // LVCS type
  auto cs_name = lvcs.get_cs_name();
  std::string cs_str = vpgl_lvcs::cs_name_strings[cs_name];

  // positive/negative longitude
  double plon = (lon > 0) ? lon : lon + 360.0;
  double nlon = (lon < 0) ? lon : lon - 360.0;

  // report
  std::cout << "\nTest " << cs_str << " lvcs around antimeridian\n"
            << "(lon, lat, elev) = (" << lon << ", "
            << lat << ", " << elev << ")\n"
            << "positive/negative lon = " << plon << "/" << nlon << "\n";

  if (cs_name == vpgl_lvcs::utm)
  {
    lvcs.get_utm_origin(easting, northing, z, utm_zone, south_flag);
    std::cout << "(easting, northing, utm_zone, south_flag) = ("
              << easting << ", " << northing << ", "
              << utm_zone << ", " << south_flag << ")\n";
  }
  std::cout << "\n";

  // WGS84 origin
  std::cout << "WGS84 origin\n";
  lvcs.get_origin(y, x, z);
  TEST_NEAR("longitude", x, lon, degree_tol);
  TEST_NEAR("latitude", y, lat, degree_tol);
  TEST_NEAR("elevation", z, elev, meter_tol);

  // local->global, offset local input, WGS84 output
  for (auto & offset : meter_offsets)
  {
    _test_lvcs_local_to_global_wgs84(lvcs, lon, lat, elev,
                                     offset[0], offset[1], offset[2],
                                     meter_tol, degree_tol);
  }

  // local->global, offset local input, UTM output
  if (cs_name == vpgl_lvcs::utm)
  {
    for (auto & offset : meter_offsets)
    {
      _test_lvcs_local_to_global_utm(lvcs, easting, northing, elev,
                                     offset[0], offset[1], offset[2],
                                     meter_tol);
    }
  }

  // global->local, offset WGS84 input, local output
  for (auto & offset : wgs84_offsets)
  {
    // standard input
    _test_lvcs_global_to_local_wgs84(lvcs, lon, lat, elev,
                                     offset[0], offset[1], offset[2],
                                     meter_tol);

    // positive longitude
    _test_lvcs_global_to_local_wgs84(lvcs, plon, lat, elev,
                                     offset[0], offset[1], offset[2],
                                     meter_tol, "positive longitude");

    // negative longitude
    _test_lvcs_global_to_local_wgs84(lvcs, nlon, lat, elev,
                                     offset[0], offset[1], offset[2],
                                     meter_tol, "negative longitude");
  }

  // global->local, offset UTM input, local output
  if (cs_name == vpgl_lvcs::utm)
  {
    for (auto & offset : meter_offsets)
    {
      _test_lvcs_global_to_local_utm(lvcs, easting, northing, elev,
                                     offset[0], offset[1], offset[2],
                                     meter_tol);
    }
  }
}


void
test_lvcs_antimeridian(double lon, double lat, double elev,
                       double meter_tol, double degree_tol)
{
  // WGS84 LVCS
  vpgl_lvcs lvcs_wgs84(lat, lon, elev, vpgl_lvcs::wgs84,
                       vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  _test_lvcs_antimeridian(lvcs_wgs84, lon, lat, elev,
                          meter_tol, degree_tol);

  // UTM LVCS
  vpgl_lvcs lvcs_utm(lat, lon, elev, vpgl_lvcs::utm,
                     vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  _test_lvcs_antimeridian(lvcs_utm, lon, lat, elev,
                          meter_tol, degree_tol);

  // UTM zone
  int utm_zone;
  bool south_flag;
  lvcs_utm.get_utm(utm_zone, south_flag);

  // For UTM zone 1, also test in UTM zone 60
  if (utm_zone == 1)
  {
    std::cout << "\nForce to UTM zone 60 (east-most zone)";
    lvcs_utm.set_utm(60, south_flag);
    _test_lvcs_antimeridian(lvcs_utm, lon, lat, elev,
                            meter_tol, degree_tol);
  }
  // For UTM zone 60, also test in UTM zone 1
  else if (utm_zone == 60)
  {
    std::cout << "\nForce to UTM zone 1 (west-most zone)";
    lvcs_utm.set_utm(1, south_flag);
    _test_lvcs_antimeridian(lvcs_utm, lon, lat, elev,
                            meter_tol, degree_tol);
  }
}


void
_test_lvcs_general(vpgl_lvcs lvcs,
                   double lon, double lat, double elev,
                   double meter_tol, double degree_tol)
{
  // results
  double x, y, z;
  double easting, northing;
  int utm_zone;
  bool south_flag;

  // meter offsets as (x, y, z)
  std::vector<std::vector<double> > meter_offsets = {
      {0, 0, 0},
      {100, 100, 100},
      {-100, -100, -100},
  };

  // WGS84 offsets as (lon, lat, elev)
  std::vector<std::vector<double> > wgs84_offsets = {
      {0, 0, 0},
      {0.01, 0.01, 100},
      {-0.01, -0.01, -100},
  };

  // LVCS type
  auto cs_name = lvcs.get_cs_name();
  std::string cs_str = vpgl_lvcs::cs_name_strings[cs_name];

  // report
  std::cout << "\nTest " << cs_str << " lvcs\n"
            << "(lon, lat, elev) = (" << lon << ", "
            << lat << ", " << elev << ")\n";

  if (cs_name == vpgl_lvcs::utm)
  {
    lvcs.get_utm_origin(easting, northing, z, utm_zone, south_flag);
    std::cout << "(easting, northing, utm_zone, south_flag) = ("
              << easting << ", " << northing << ", "
              << utm_zone << ", " << south_flag << ")\n";
  }
  std::cout << "\n";

  // WGS84 origin
  std::cout << "WGS84 origin\n";
  lvcs.get_origin(y, x, z);
  TEST_NEAR("longitude", x, lon, degree_tol);
  TEST_NEAR("latitude", y, lat, degree_tol);
  TEST_NEAR("elevation", z, elev, meter_tol);

  // local->global, offset local input, WGS84 output
  for (auto & offset : meter_offsets)
  {
    _test_lvcs_local_to_global_wgs84(lvcs, lon, lat, elev,
                                     offset[0], offset[1], offset[2],
                                     meter_tol, degree_tol);
  }

  // local->global, offset local input, UTM output
  if (cs_name == vpgl_lvcs::utm)
  {
    for (auto & offset : meter_offsets)
    {
      _test_lvcs_local_to_global_utm(lvcs, easting, northing, elev,
                                     offset[0], offset[1], offset[2],
                                     meter_tol);
    }
  }

  // global->local, offset WGS84 input, local output
  for (auto & offset : wgs84_offsets)
  {
    _test_lvcs_global_to_local_wgs84(lvcs, lon, lat, elev,
                                     offset[0], offset[1], offset[2],
                                     meter_tol);
  }

  // global->local, offset UTM input, local output
  if (cs_name == vpgl_lvcs::utm)
  {
    for (auto & offset : meter_offsets)
    {
      _test_lvcs_global_to_local_utm(lvcs, easting, northing, elev,
                                     offset[0], offset[1], offset[2],
                                     meter_tol);
    }
  }
}


void
test_lvcs_general(double lon, double lat, double elev,
                  double meter_tol, double degree_tol,
                  int extra_utm_zone=-1, int extra_south_flag=-1)
{
  // WGS84 LVCS
  vpgl_lvcs lvcs_wgs84(lat, lon, elev, vpgl_lvcs::wgs84,
                       vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  _test_lvcs_general(lvcs_wgs84, lon, lat, elev,
                     meter_tol, degree_tol);

  // UTM LVCS
  vpgl_lvcs lvcs_utm(lat, lon, elev, vpgl_lvcs::utm,
                     vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  _test_lvcs_general(lvcs_utm, lon, lat, elev,
                     meter_tol, degree_tol);

  // Also force into neighboring utm zone & south_flag
  if (extra_utm_zone != -1)
  {
    std::cout << "\nAlso force to UTM zone " << extra_utm_zone
              << " with south flag " << extra_south_flag << "\n";
    lvcs_utm.set_utm(extra_utm_zone, extra_south_flag);
    _test_lvcs_general(lvcs_utm, lon, lat, elev,
                       meter_tol, degree_tol);
  }
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


  // ----- Prime Meridian/Equator -----

  // exactly at lon/lat=0/0
  test_lvcs_general( 0.0,  0.0, 100.0, meter_tol, degree_tol);

  // around prime meridian & equator
  // including UTM tests in neighboring utm_zone & south_flag
  test_lvcs_general(-0.001,  0.001, 100.0, meter_tol, degree_tol, 31, 1);
  test_lvcs_general( 0.001,  0.001, 100.0, meter_tol, degree_tol, 30, 1);
  test_lvcs_general( 0.001, -0.001, 100.0, meter_tol, degree_tol, 30, 0);
  test_lvcs_general(-0.001, -0.001, 100.0, meter_tol, degree_tol, 31, 0);


  // ----- Antimeridian -----
  // Test points near the antimeridian, where longitude changes from
  // 180 degrees to -180 degrees.
  test_lvcs_antimeridian(-179.0, 71.0, 100.0, meter_tol, degree_tol);
  test_lvcs_antimeridian( 179.0, 71.0, 100.0, meter_tol, degree_tol);
  test_lvcs_antimeridian(-181.0, 71.0, 100.0, meter_tol, degree_tol);
  test_lvcs_antimeridian( 181.0, 71.0, 100.0, meter_tol, degree_tol);

}

TESTMAIN(test_lvcs);
