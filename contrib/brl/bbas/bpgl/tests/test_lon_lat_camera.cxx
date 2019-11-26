#include <iostream>
#include <fstream>
#include <sstream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/vpgl_affine_camera.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_polygon.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <bpgl/bpgl_lon_lat_camera.h>
static void test_lon_lat_camera()
{
#if 0

  std::string lon_path = dir + "longitude.tif";
  std::string lat_path = dir + "latitude.tif";
  std::string img_path = dir + "rgb.tif";
  std::string dsm_path = "D:/tests/aces_test/prob_dsm_1.0m.tif";
  std::string err_path = dir + "err_map.tif";
  bpgl_lon_lat_camera llc;
  bool good = llc.init_image(img_path, lon_path, lat_path);
  good == good && llc.fit_geo_to_uv_map();
  good == good && llc.save_error_image(err_path);
#endif
}
TESTMAIN(test_lon_lat_camera);
