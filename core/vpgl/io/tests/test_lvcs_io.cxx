#include <iostream>
#include <cmath>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/io/vpgl_io_lvcs.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>

static void test_lvcs_io()
{
  vpgl_lvcs lvcs(33.4447732, -114.3085932, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::FEET);
  double x,y,z;
  lvcs.global_to_local(-114.3085932, 33.4447732, 0.0,vpgl_lvcs::wgs84, x, y, z);
  std::string b_path = "./test_lvcs_io.vsl";
  vsl_b_ofstream os(b_path);
  vsl_b_write(os, lvcs);
  os.close();
  vsl_b_ifstream is(b_path);
  vpgl_lvcs lvcs_r;
  vsl_b_read(is, lvcs_r);
  double longitude, latitude, elevation;
  lvcs_r.get_origin(latitude, longitude, elevation);
  vpgl_lvcs::cs_names name = lvcs_r.get_cs_name();
  double er1 = std::fabs(longitude+114.3085932)+
    std::fabs(latitude-33.4447732) + std::fabs(elevation);
  TEST("cs_name", name-vpgl_lvcs::wgs84, 0);
  TEST_NEAR("origin", er1, 0.0, 1e-3);
  double xr, yr, zr;
  lvcs.global_to_local(-114.3085932, 33.4447732, 0.0,vpgl_lvcs::wgs84, xr, yr, zr);
  double er2 = std::fabs(xr)+std::fabs(yr)+std::fabs(zr);
  TEST_NEAR("local", er2, 0.0, 1e-3);
  vpl_unlink("./test_lvcs_io.vsl");
}

TESTMAIN(test_lvcs_io);
