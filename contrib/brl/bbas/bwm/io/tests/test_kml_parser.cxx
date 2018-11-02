#include <string>
#include <iostream>
#include <cstdio>
#include <testlib/testlib_test.h>
#include "../bwm_io_kml_camera.h"
#include <expatpp.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
void test_kml_parser()
{
  std::string path = "";//FIXME need test kml file
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
