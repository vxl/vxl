#include <iostream>
#include <fstream>
#include <sstream>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "vpgl/vpgl_affine_camera.h"
#include "vgl/vgl_vector_3d.h"
#include "vgl/vgl_polygon.h"
#include "vil/vil_image_resource.h"
#include "vil/vil_load.h"
#include "vpgl/vpgl_local_rational_camera.h"
#include <bpgl/bpgl_geotif_camera.h>
static void test_geotif_camera()
{
  // RPC camera is from opensource Buenos Aires DSM challenge dataset
  // https://spacenetchallenge.github.io/
    std::string rpb = "satId = \"????\";\n";
    rpb += "bandId = \"RGB\" \n";
    rpb += "SpecId = \"RPC00B\" \n";
    rpb += "BEGIN_GROUP = IMAGE \n";
    rpb += "lineOffset = 3854.000000 \n";
    rpb += "sampOffset = 5051.000000 \n";
    rpb += "latOffset = -34.475400 \n";
    rpb += "longOffset = -58.611000 \n";
    rpb += "heightOffset = 31.000000 \n";
    rpb += "lineScale = 14360.000000 \n";
    rpb += "sampScale = 13617.000000 \n";
    rpb += "latScale = 0.068800 \n";
    rpb += "longScale = 0.075800 \n";
    rpb += "heightScale = 500.000000 \n";
    rpb += "lineNumCoef = ( \n";
    rpb += "-1.155039000000e-02, \n";
    rpb += "-2.219071000000e-02, \n";
    rpb += "+1.037603000000e+00, \n";
    rpb += "-1.430930000000e-02, \n";
    rpb += "+1.430638000000e-03, \n";
    rpb += "-4.002035000000e-05, \n";
    rpb += "+6.988066000000e-04, \n";
    rpb += "-2.803232000000e-04, \n";
    rpb += "+8.815574000000e-03, \n";
    rpb += "-9.492739000000e-06, \n";
    rpb += "+1.691069000000e-06, \n";
    rpb += "+7.474433000000e-07, \n";
    rpb += "+1.716209000000e-05, \n";
    rpb += "+1.368658000000e-06, \n";
    rpb += "-5.536113000000e-05, \n";
    rpb += "-1.669293000000e-04, \n";
    rpb += "-6.567348000000e-05, \n";
    rpb += "+4.828972000000e-07, \n";
    rpb += "+2.179128000000e-05, \n";
    rpb += "+9.029418000000e-07); \n";
    rpb += "lineDenCoef = ( \n";
    rpb += "+1.000000000000e+00, \n";
    rpb += "-1.748998000000e-03, \n";
    rpb += "+2.867378000000e-03, \n";
    rpb += "-8.918910000000e-04, \n";
    rpb += "-1.100900000000e-05, \n";
    rpb += "-3.130936000000e-06, \n";
    rpb += "-1.505779000000e-05, \n";
    rpb += "-4.256269000000e-05, \n";
    rpb += "+1.791118000000e-04, \n";
    rpb += "-6.411448000000e-05, \n";
    rpb += "-1.480182000000e-07, \n";
    rpb += "+1.884002000000e-07, \n";
    rpb += "+2.299439000000e-06, \n";
    rpb += "+2.065444000000e-07, \n";
    rpb += "+1.937789000000e-07, \n";
    rpb += "+6.022982000000e-05, \n";
    rpb += "+3.128753000000e-07, \n";
    rpb += "+9.631539000000e-08, \n";
    rpb += "-1.065953000000e-06, \n";
    rpb += "+1.067070000000e-07); \n";
    rpb += "sampNumCoef = ( \n";
    rpb += "+3.072148000000e-03, \n";
    rpb += "-1.015328000000e+00, \n";
    rpb += "+1.564052000000e-04, \n";
    rpb += "+7.921286000000e-03, \n";
    rpb += "-3.215768000000e-03, \n";
    rpb += "-3.729291000000e-04, \n";
    rpb += "-2.210835000000e-04, \n";
    rpb += "-2.129823000000e-03, \n";
    rpb += "+1.602836000000e-05, \n";
    rpb += "+5.707117000000e-06, \n";
    rpb += "-9.895511000000e-06, \n";
    rpb += "+3.067685000000e-05, \n";
    rpb += "+1.623712000000e-04, \n";
    rpb += "+2.474425000000e-05, \n";
    rpb += "-1.358906000000e-05, \n";
    rpb += "-2.529502000000e-04, \n";
    rpb += "-5.387624000000e-07, \n";
    rpb += "-1.077882000000e-06, \n";
    rpb += "+7.690791000000e-06, \n";
    rpb += "-1.855927000000e-07); \n";
    rpb += "sampDenCoef = ( \n";
    rpb += "+1.000000000000e+00, \n";
    rpb += "+9.596563000000e-04, \n";
    rpb += "-3.263896000000e-03, \n";
    rpb += "-4.033469000000e-04, \n";
    rpb += "+9.097765000000e-06, \n";
    rpb += "-1.200640000000e-06, \n";
    rpb += "-9.518890000000e-06, \n";
    rpb += "-2.531124000000e-05, \n";
    rpb += "+1.419847000000e-04, \n";
    rpb += "-2.445755000000e-05, \n";
    rpb += "-9.716162000000e-08, \n";
    rpb += "+4.391155000000e-08, \n";
    rpb += "+2.657253000000e-06, \n";
    rpb += "+2.810917000000e-08, \n";
    rpb += "+1.262118000000e-07, \n";
    rpb += "+3.400646000000e-06, \n";
    rpb += "+1.128371000000e-07, \n";
    rpb += "+2.805515000000e-08, \n";
    rpb += "+8.268550000000e-07, \n";
    rpb += "+1.966835000000e-08); \n";
    rpb += "END_GROUP = IMAGE \n";
    rpb += "END; \n";
    rpb += "lvcs \n";
    rpb += "-58.5856405 \n";
    rpb += "-34.49092225 \n";
    rpb += "-23.7482891083 \n";
    std::stringstream ss(rpb);

  // UTM example onstruct from a matrix no external files needed
  vpgl_local_rational_camera<float> mrcam;
  ss >> mrcam;
  vnl_matrix<float> tm(4,4,0.0f);
  tm[0][0] = 0.3f;   tm[1][1] = -0.3f; tm[0][3] = 354407.24936f;
  tm[1][3] = 6182480.621208f; tm[3][3]=1.0f;
  int utm_zone = 21;
  int northing = 1;
  float mx = 170.7f, my= 103.5f, mz = 53.1f;
  float mtifu = 575.0f, mtifv = 365.0f, mtifz = 52.83f;
  float mu, mv;
  bpgl_geotif_camera<float> gcam3;
  vpgl_lvcs_sptr null_ptr;
  bool bgood = gcam3.construct_from_matrix(mrcam, tm, false, null_ptr, northing, utm_zone);
  gcam3.project(mx, my, mz, mu, mv);
  std::cout << "MATRIX_UTM(u, v) " << mu << ' ' << mv << std::endl;
  gcam3.project_gtif_to_image(mtifu, mtifv, mtifz, mu, mv);
  std::cout << "MATRIX_DSM_UTM(u, v) " << mu << ' ' << mv << std::endl;
  bgood = bgood && fabs(mu - 137.3) < 0.1;
  TEST("geotiff camera projection", bgood, true);
  bool is_utm = gcam3.is_utm();
  bool eorg_at_zero= gcam3.elevation_origin_at_zero();
  bool has_lvcs = gcam3.has_lvcs();
  bool proj_local = gcam3.project_local_points();
  TEST("bool accessors", is_utm&&!eorg_at_zero&&has_lvcs&& proj_local, true);
}
TESTMAIN(test_geotif_camera);
