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

  vcl_cout << "number of hypotheses: " << hyp.size() << " approx size on RAM: " << hyp.size()*sizeof(float)*3.0/(1024.0) << " KBs. " << vcl_endl;
  
  // io test
  vcl_string hyp_file_name = "./loc_hyp_test.bin";
  vcl_string hyp_file_kml  = "./loc_hyp_test.kml";
  hyp.write_hypotheses(hyp_file_name);
  hyp.write_to_kml(hyp_file_kml, 0, true);

  volm_loc_hyp hyp2(hyp_file_name);
  TEST("binary io test", hyp.size(), hyp2.size());

  // point fetch methods
  vgl_point_3d<double> pt;
  hyp2.get_next(pt);
  vcl_cout << " first point is: " << pt << vcl_endl;
  for (unsigned i = 1; i < hyp2.size(); i++)
    hyp2.get_next(pt);
  hyp2.get_next(pt);
  vcl_cout << " last point is:  " << pt << vcl_endl;
  TEST("got all points", hyp2.get_next(pt), false);

  // get the closest point
  volm_loc_hyp hyp3(hyp_file_name);
  double lat = 38.0, lon = -118.0;
  unsigned hyp_id;
  vgl_point_3d<double> closest_loc;
  hyp3.get_closest(lat, lon, closest_loc, hyp_id);
  vcl_cout << " closest point to location " << lon << ", " << lat << " is: " << closest_loc << " (hyp id is " << hyp_id << ')' << vcl_endl;
  TEST("closest locs", hyp_id, 2);

#if 0
# if 1  // create index
  // big scene - DESERT WORLD
  /*double x[] = {-118.21f, -117.102f, -117.102f, -118.21f, -118.21f};
  double y[] = {38.553f, 38.553f, 37.623f, 37.623f, 38.553f};
  vgl_polygon<double> poly(x, y, 5);*/
  // big scene - simple square Coast poly
  double x[] = {-87.0f, -73.0f, -69.0f, -83.0f, -87.0f};
  double y[] = {28.0f, 26.0f, 37.0f, 39.00f, 28.0f};
  vgl_polygon<double> poly(x, y, 5);


  // lvcs of world2
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(38.074326, -117.654869, 1200.0, vpgl_lvcs::utm, 0.0, 0.0, vpgl_lvcs::DEG, vpgl_lvcs::METERS, 0.0, 0.0, 0.0);
  int pixel_inc_in_dem = 1000;
  //vcl_string hyp_file_name = "C:/projects/FINDER/simple_index/nam_dem_with_gt_locs_100_100.bin";
  vcl_string hyp_file_name = "C:/projects/FINDER/simple_index/nam_dem_locs_100_100.bin";
  //vcl_string hyp_file_name = "C:/projects/FINDER/simple_index/nam_dem_gt_locs.bin";
  
  /* scene_larger
  vcl_string poly_file = "C:/projects/FINDER/desert/res03/poly1.kml";
  vgl_polygon<double> poly = bkml_parser::parse_polygon(poly_file); // in this poly lat is x, lon is y, switch it
  //vgl_polygon<double> poly(1);
  //for (unsigned i = 0; i < poly_r[0].size(); i++) {
  //  vgl_point_2d<double>pt(poly_r[0][i].y(), poly_r[0][i].x());
  //  poly[0].push_back(pt);
  //}
  vcl_cout << poly << vcl_endl;
  vcl_cout << "outer poly size: " << poly[0].size() << vcl_endl;
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(37.961048, -117.988785, 1400.0, vpgl_lvcs::utm, 0.0, 0.0, vpgl_lvcs::DEG, vpgl_lvcs::METERS, 0.0, 0.0, 0.0);  
  int pixel_inc_in_dem = 100;
  vcl_string hyp_file_name = "C:/projects/FINDER/simple_index/smaller_scene_nam_dem_locs_100_100.bin";
  */
  vcl_string data_folder = "I:/";
  /*
  //vcl_string data_folder = "/home/ozge/mnt/fileserver/projects/FINDER/";
  vcl_string dem_file = data_folder + "P-1A/Finder_Program_Only/ReferenceData/Desert/NAMDTM/NAM_DEM.tif";
  vil_image_resource_sptr dem_res = vil_load_image_resource(dem_file.c_str());
  vil_image_view<float> dem = dem_res->get_view();
  vpgl_geo_camera* geocam = 0;
  vpgl_geo_camera::init_geo_camera(dem_res, lvcs, geocam);
  */

  vcl_string dem_file = data_folder + "Public_LIDAR/2_meter_lidar_tiles/lidar_N30.3125W081.4375_S0.0625x0.0625.tif";
  vil_image_resource_sptr dem_res = vil_load_image_resource(dem_file.c_str());
  vil_image_view<float> dem = dem_res->get_view();
  vpgl_geo_camera *geocam = 0;
  vpgl_geo_camera::init_geo_camera(dem_file, dem.ni(), dem.nj(), lvcs, geocam);

  volm_loc_hyp hyp;
  // for desert
  //hyp.add(poly, dem, geocam, pixel_inc_in_dem, pixel_inc_in_dem);
  // for coast
  hyp.add(poly, dem, geocam, pixel_inc_in_dem, pixel_inc_in_dem, true, 'N', 'W');
  //volm_loc_hyp hyp;
  /*
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
  hyp.add(37.7082748413, -117.416511536, 1860.99194336);*/

  vcl_cout << "number of hypotheses: " << hyp.size() << " approx size on RAM: " << hyp.size()*sizeof(float)*3.0/(1024*1024*1024) << " GBs." << vcl_endl;
  hyp.write_hypotheses(hyp_file_name);
  vcl_cout << "size on file: " << vul_file::size(hyp_file_name) << vcl_endl;
#endif

  //vcl_string hyp_file_name = "C:/projects/FINDER/simple_index/nam_dem_locs_1000_1000.bin";
  volm_loc_hyp hyp2(hyp_file_name);
  //volm_loc_hyp hyp2("C:/projects/FINDER/simple_index/nam_dem_locs_1_1.bin");
  vcl_cout << "hyp2 has " << hyp2.size() << " hyps!n";
  //TEST("size of loaded hyp", hyp2.size(), hyp.size());
  /*
  vgl_point_3d<float> pt, pt2,pt3, pt4;
  hyp2.get_next(pt);
  vcl_cout << " first point is: " << pt.x() << ' ' << pt.y() << ' ' << pt.z() << vcl_endl;
  for (unsigned i = 1; i < hyp2.size()-1; i++)
    hyp2.get_next(pt);
  hyp2.get_next(pt);
  vcl_cout << " last point is: " << pt.x() << ' ' << pt.y() << ' ' << pt.z() << vcl_endl;
  TEST("got all", hyp2.get_next(pt), false);
  volm_loc_hyp hyp3(hyp_file_name);
  unsigned start = 50; unsigned skip = 10;
  for (unsigned i = 0; i <= start; i++) 
    hyp3.get_next(pt);
  for (unsigned i = 0; i < skip; i++) 
    hyp3.get_next(pt3);
  
  volm_loc_hyp hyp4(hyp_file_name);
  hyp4.get_next(start, skip, pt2);
  TEST("skipped 50 ", pt, pt2);
  
  volm_loc_hyp hyp5(hyp_file_name);
  hyp5.get_next(start, skip, pt4);
  hyp5.get_next(start, skip, pt4);
  TEST("skipped 50 ", pt3, pt4);
 */ 
#endif
}


TESTMAIN(test_loc_hyp);
