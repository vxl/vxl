#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vsl/vsl_binary_io.h>
#include <vul/vul_timer.h>
#include <bbas/volm/volm_loc_hyp.h>
#include <vgl/vgl_polygon.h>
#include <bkml/bkml_parser.h>

static void test_loc_hyp()
{
  // create a location hyp and add certain number of locations
  volm_loc_hyp hyp;
  hyp.add(37.6763153076, -117.978889465, 1891.40698242);
  hyp.add(37.6763076782, -117.978881836, 1891.40698242);
  hyp.add(37.9629554749, -117.964019775, 1670.87597656);
  hyp.add(37.9623947144, -117.962753296, 1643.76293945);
  hyp.add(38.5223121643, -118.147850037, 1382.84594727);
  hyp.add(38.5202674866, -118.147697449, 1378.42004395);
  hyp.add(38.0597877502, -117.936508179, 1376.12097168);
  hyp.add(38.0598144531, -117.936523438, 1376.12097168);
  hyp.add(37.7081604004, -117.416557312, 1860.39599609);
  hyp.add(37.7084999084, -117.416664124, 1863.23205566);
  hyp.add(37.7082748413, -117.416511536, 1860.99194336);

  std::cout << "number of hypotheses: " << hyp.size() << " approx size on RAM: " << hyp.size()*sizeof(float)*3.0/(1024.0) << " KBs. " << std::endl;

  // io test
  std::string hyp_file_name = "./loc_hyp_test.bin";
  std::string hyp_file_kml  = "./loc_hyp_test.kml";
  hyp.write_hypotheses(hyp_file_name);
  hyp.write_to_kml(hyp_file_kml, 0, true);

  volm_loc_hyp hyp2(hyp_file_name);
  TEST("binary io test", hyp.size(), hyp2.size());

  // point fetch methods
  vgl_point_3d<double> pt;
  hyp2.get_next(pt);
  std::cout << " first point is: " << pt << std::endl;
  for (unsigned i = 1; i < hyp2.size(); i++)
    hyp2.get_next(pt);
  hyp2.get_next(pt);
  std::cout << " last point is:  " << pt << std::endl;
  TEST("got all points", hyp2.get_next(pt), false);

  // get the closest point
  volm_loc_hyp hyp3(hyp_file_name);
  double lat = 38.0, lon = -118.0;
  unsigned hyp_id;
  vgl_point_3d<double> closest_loc;
  hyp3.get_closest(lat, lon, closest_loc, hyp_id);
  std::cout << " closest point to location " << lon << ", " << lat << " is: " << closest_loc << " (hyp id is " << hyp_id << ')' << std::endl;
  TEST("closest locs", hyp_id, 2);


}


TESTMAIN(test_loc_hyp);
