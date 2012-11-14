#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vsl/vsl_binary_io.h>
#include <vul/vul_timer.h>
#include <bbas/volm/volm_loc_hyp.h>
#include <vgl/vgl_polygon.h>

static void test_loc_hyp()
{
# if 0  // create index
  float x[] = {-118.21f, -117.102f, -117.102f, -118.21f, -118.21f};
  float y[] = {38.553f, 38.553f, 37.623f, 37.623f, 38.553f};
  vgl_polygon<float> poly(x, y, 5);

  // lvcs of world2
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(38.074326, -117.654869, 1200.0, vpgl_lvcs::utm, 0.0, 0.0, vpgl_lvcs::DEG, vpgl_lvcs::METERS, 0.0, 0.0, 0.0);
  vcl_string dem_file = "D:/P-1A/Finder_Program_Only/ReferenceData/Desert/NAMDTM/NAM_DEM.tif";
  vil_image_resource_sptr dem_res = vil_load_image_resource(dem_file.c_str());
  vil_image_view<float> dem = dem_res->get_view();
  vpgl_geo_camera* geocam = 0;
  vpgl_geo_camera::init_geo_camera(dem_res, lvcs, geocam);

  volm_loc_hyp hyp(poly, dem, geocam, 5000, 5000);
  //volm_loc_hyp hyp(poly, dem, geocam, 1, 1);

  // add gt positions
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

  vcl_cout << "number of hypotheses: " << hyp.size() << " approx size on RAM: " << hyp.size()*sizeof(float)*3.0/(1024*1024*1024) << " GBs." << vcl_endl;
  hyp.write_hypotheses("C:/projects/FINDER/simple_index/nam_dem_locs_5000_5000.bin");
  vcl_cout << "size on file: " << vul_file::size("C:/projects/FINDER/simple_index/nam_dem_locs_5000_5000.bin") << vcl_endl;


#endif
  volm_loc_hyp hyp2("C:/projects/FINDER/simple_index/nam_dem_locs_5000_5000.bin");
  //volm_loc_hyp hyp2("C:/projects/FINDER/simple_index/nam_dem_locs_1_1.bin");
  vcl_cout << "hyp2 has " << hyp2.size() << " hyps!n";
  //TEST("size of loaded hyp", hyp2.size(), hyp.size());

  vgl_point_3d<float> pt;
  hyp2.get_next(pt);
  vcl_cout << " first point is: " << pt.x() << ' ' << pt.y() << ' ' << pt.z() << vcl_endl;
  for (unsigned i = 1; i < hyp2.size()-1; i++)
    hyp2.get_next(pt);
  hyp2.get_next(pt);
  vcl_cout << " last point is: " << pt.x() << ' ' << pt.y() << ' ' << pt.z() << vcl_endl;
  TEST("got all", hyp2.get_next(pt), false);
}


TESTMAIN(test_loc_hyp);
