#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bpgl/bpgl_camera_utils.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_polygon.h>
static void test_camera_utils()
{
  // =============  test kml camera constructor========================
  unsigned ni = 1280, nj = 720;
  auto nid = static_cast<double>(ni), njd = static_cast<double>(nj);
  double right_fov = 29.605;
  double top_fov = 17.725;
  double altitude = 1.6;
  double heading = 170.65;
  double tilt = 78.62;
  double roll = -2.76;
  vpgl_perspective_camera<double> cam =
    bpgl_camera_utils::camera_from_kml(nid, njd, right_fov, top_fov,
                                       altitude, heading, tilt, roll);
  vgl_vector_3d<double> pray = cam.principal_axis();
  //s,c == sin,cos(head), st == sin(tilt)
  double s = 0.1625, c = -0.9867, st = 0.9803;
  double dif = std::fabs(pray.x()-(s*st));
  dif += std::fabs(pray.y()-(c*st));
  TEST_NEAR("principal ray (KML)", dif, 0.0, 0.0001);
}


TESTMAIN(test_camera_utils);
