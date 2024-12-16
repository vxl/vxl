#include <iostream>
#include <cmath>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vpgl/vpgl_lvcs.h"
#include "vpgl/vpgl_lvcs_sptr.h"
#include <vpgl/io/vpgl_io_lvcs.h>
#include "vpl/vpl.h"
#include "vsl/vsl_binary_io.h"

void
_test_lvcs()
{
  vpgl_lvcs lvcs(33.4447732, -114.3085932, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::FEET);
  double x, y, z;
  lvcs.global_to_local(-114.3085932, 33.4447732, 0.0, vpgl_lvcs::wgs84, x, y, z);
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
  double er1 = std::fabs(longitude + 114.3085932) + std::fabs(latitude - 33.4447732) + std::fabs(elevation);
  TEST("cs_name", name - vpgl_lvcs::wgs84, 0);
  TEST_NEAR("origin", er1, 0.0, 1e-3);
  double xr, yr, zr;
  lvcs.global_to_local(-114.3085932, 33.4447732, 0.0, vpgl_lvcs::wgs84, xr, yr, zr);
  double er2 = std::fabs(xr) + std::fabs(yr) + std::fabs(zr);
  TEST_NEAR("local", er2, 0.0, 1e-3);
  vpl_unlink("./test_lvcs_io.vsl");
}

void
_test_lvcs_sptr_wgs84()
{
  std::string filename = "test_lvcs_sptr_wgs84_io.vsl";

  auto lvcs_sptr =
    vpgl_lvcs_sptr(new vpgl_lvcs(33.4447732, -114.3085932, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::FEET));
  std::cout << "vpgl_lvcs_sptr wgs84 : " << *lvcs_sptr;

  vsl_b_ofstream os(filename);
  vsl_b_write(os, lvcs_sptr);
  os.close();

  vsl_b_ifstream is(filename);
  vpgl_lvcs_sptr lvcs_r = nullptr;
  vsl_b_read(is, lvcs_r);
  is.close();

  TEST("vpgl_lvcs_sptr", *lvcs_r, *lvcs_sptr);

  vpl_unlink(filename.c_str());
}

void
_test_lvcs_sptr_utm()
{
  std::string filename = "test_lvcs_sptr_utm_io.vsl";

  auto lvcs_sptr =
    vpgl_lvcs_sptr(new vpgl_lvcs(33.4447732, -114.3085932, 0.0, vpgl_lvcs::utm, vpgl_lvcs::DEG, vpgl_lvcs::METERS));
  std::cout << "vpgl_lvcs_sptr utm : " << *lvcs_sptr;

  vsl_b_ofstream os(filename);
  vsl_b_write(os, lvcs_sptr);
  os.close();

  vsl_b_ifstream is(filename);
  vpgl_lvcs_sptr lvcs_r = nullptr;
  vsl_b_read(is, lvcs_r);
  is.close();

  TEST("vpgl_lvcs_sptr", *lvcs_r, *lvcs_sptr);

  vpl_unlink(filename.c_str());
}

void
_test_lvcs_sptr_null()
{
  std::string filename = "test_lvcs_sptr_null_io.vsl";

  vsl_b_ofstream os(filename);
  os.close();

  vsl_b_ifstream is(filename);
  vpgl_lvcs_sptr lvcs_r = nullptr;
  vsl_b_read(is, lvcs_r);
  is.close();

  TEST("vpgl_lvcs_sptr null", lvcs_r, nullptr);

  vpl_unlink(filename.c_str());
}

static void
test_lvcs_io()
{
  _test_lvcs();
  _test_lvcs_sptr_wgs84();
  _test_lvcs_sptr_utm();
  _test_lvcs_sptr_null();
}

TESTMAIN(test_lvcs_io);
