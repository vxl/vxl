#include <testlib/testlib_test.h>
#include "../bwm_io_kml_camera.h"
#include <expatpp.h>
#include <vcl_string.h>
#include <vcl_cstdio.h>
#include <vcl_iostream.h>
void test_kml_parser()
{
  vcl_string path = "";//FIXME need test kml file
  double right_fov;
  double top_fov;
  double altitude;
  double heading;
  double tilt;
  double roll;
  bwm_io_kml_camera::read_camera(path, right_fov, top_fov, altitude,
                                 heading, tilt, roll);

}
TESTMAIN(test_kml_parser);
