#include <iostream>
#include <fstream>
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
#include <bpgl/bpgl_geotif_camera.h>
static void test_geotif_camera()
{
  float lat = 35.656769024, lon = -117.660635516, elev = 665.0;
  float x = 230.0f, y = 194.0f, z = 668.f;
  float lon_lvcs = -117.66319151485095f;
  float lat_lvcs = 35.65505041186045f;
  float elev_lvcs = 0.0;
  vpgl_lvcs_sptr local_lvcs = new vpgl_lvcs(lat_lvcs, lon_lvcs, elev_lvcs, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  std::string path = "D:/tests/china_lake/results/DSM.tif";
  vil_image_resource_sptr resc = vil_load_image_resource(path.c_str());
  std::string cam_path = "D:/tests/china_lake/results/crop/tile_4/07DEC19182724-P1BS-010928506010_01_P001.rpb";
  vpgl_local_rational_camera<float> rcam;
  std::ifstream istr(cam_path.c_str());
  istr >> rcam;
  vpgl_camera<float>& cam = dynamic_cast<vpgl_rational_camera<float>&>(rcam);
  std::cout << cam.is_a() << std::endl;
  bpgl_geotif_camera<float> gcam;
  bool good = gcam.construct_from_geotif(cam, resc);
  gcam.set_elevation_offset(-601.071594238);
  float u = 0.0f, v= 0.0f;
  gcam.project(x, y, z, u, v);
  std::cout << "(u, v) " << u << ' ' << v << std::endl;
  float tifu = 840.0f, tifv = 420.0f, tifz = 668.0f;
  gcam.project_gtif_to_image(tifu, tifv, tifz, u, v);
  std::cout << "dsm(u, v) " << u << ' ' << v << std::endl;

  // UTM example
  path = "D:/tests/buenos_aires/results_before_noah/tmp/crop/tile_13/stereo_dsms/geo_fused_step_smoothed.tif";
  resc = vil_load_image_resource(path.c_str());
  cam_path = "D:/tests/buenos_aires/results_before_noah/tmp/crop/tile_13/14NOV15135121-P1BS-500171606160_05_P005.rpb";
  vpgl_local_rational_camera<float> rcam2;
  std::ifstream istr2(cam_path.c_str());
  istr2 >> rcam2;
  vpgl_camera<float>& cam2 = dynamic_cast<vpgl_camera<float>&>(rcam2);
  bpgl_geotif_camera<float> gcam2;
  good = gcam2.construct_from_geotif(cam2, resc);
  x = 170.7; y= 103.5; z = 53.1;
  lon = -58.5856405; lat = -34.49092225; elev = -23.7482891083;
  double plon = -58.583770293, plat = -34.490005892, pelev = 52.797;
  //-58.583782026922073 -34.489989236483375 29.351709365821094
 // -58.5856400 -34.4909210
  gcam2.set_elevation_offset(23.7482891083);
  gcam2.project(x, y, z, u, v);
  std::cout << "(u, v) " << u << ' ' << v << std::endl;
  tifu = 575; tifv = 365; tifz = 52.83;
  gcam2.project_gtif_to_image(tifu, tifv, tifz, u, v);
  std::cout << "DSM_UTM(u, v) " << u << ' ' << v << std::endl;
  
}
TESTMAIN(test_geotif_camera);
