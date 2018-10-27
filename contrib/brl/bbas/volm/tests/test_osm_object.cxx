#include <testlib/testlib_test.h>
#include <volm/volm_osm_object_point.h>
#include <volm/volm_osm_object_line.h>
#include <volm/volm_osm_object_polygon.h>
#include <volm/volm_osm_objects.h>
#include <vcl_where_root_dir.h>
#include <vul/vul_file.h>

static void test_osm_object_point()
{
  vgl_point_2d<double> loc(12.31, 19.80);
  volm_land_layer prop(1,"Open_water",0, 2.0);
  volm_osm_object_point_sptr osm_loc = new volm_osm_object_point(prop, loc);
  // test the binary io
  vsl_b_ofstream os("./volm_osm_point.bin");
  vsl_b_write(os, osm_loc);
  os.close();
  volm_osm_object_point_sptr osm_loc_in;
  vsl_b_ifstream is("./volm_osm_point.bin");
  vsl_b_read(is, osm_loc_in);
  osm_loc->print();
  osm_loc_in->print();
  TEST("osm_object_point io", osm_loc->loc(), osm_loc_in->loc());
  is.close();
}

static void test_osm_object_line()
{
  std::vector<vgl_point_2d<double> > line;
  for (unsigned i = 0; i < 10; i++)
    line.emplace_back(12.31*i, 19.80*i);
  volm_land_layer prop(2, "invalid", 0, 1.0);
  volm_osm_object_line_sptr osm_road = new volm_osm_object_line(prop, line);
  // test binary io
  vsl_b_ofstream os("./volm_osm_line.bin");
  vsl_b_write(os, osm_road);
  os.close();

  volm_osm_object_line_sptr osm_road_in;
  vsl_b_ifstream is("./volm_osm_line.bin");
  vsl_b_read(is, osm_road_in);
  is.close();
  std::cout << " origin: \n";
  osm_road->print();
  std::cout << " loaded: \n";
  osm_road->print();
  TEST("osm_object_line io", osm_road->line().size(), osm_road_in->line().size());
}

static void test_osm_object_polygon()
{
  vgl_polygon<double> poly;
  for (unsigned i = 0; i < 2; i++) {
    poly.new_sheet();
    for (unsigned j = 0; j < 6; j++)
      poly.push_back(vgl_point_2d<double>(1.2*j+i, 3.1+i+j));
  }
  volm_land_layer prop(3, "temp", 0, 3.0);
  volm_osm_object_polygon_sptr osm_region = new volm_osm_object_polygon(prop, poly);
  // test binary io
  osm_region->print();

  vsl_b_ofstream os("./volm_osm_polygon.bin");
  vsl_b_write(os, osm_region);
  os.close();

  volm_osm_object_polygon_sptr osm_region_in;
  vsl_b_ifstream is("./volm_osm_polygon.bin");
  vsl_b_read(is, osm_region_in);
  is.close();
  std::cout << " loaded osm_polygon:\n";
  osm_region_in->print();
  TEST("osm_object_polygon io", osm_region->poly().num_sheets(), osm_region_in->poly().num_sheets());
}

static void test_load_osm(std::string const& osm_file)
{
  std::string osm_to_volm_file = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/osm_to_volm_labels.txt";
  // create a volm_osm_objects
  volm_osm_objects objs(osm_file, osm_to_volm_file);
  std::cout << " parsing osm file: " << osm_file << std::endl;
  std::cout << " number of location points parsed from osm: " << objs.num_locs() << std::endl;
  std::cout << " number of line roads parsed from osm: " << objs.num_roads() << std::endl;
  std::cout << " number of regions parsed from osm: " << objs.num_regions() << std::endl;
  TEST("parsing locations points from osm", objs.num_locs(), 1);
  TEST("parsing line from osm", objs.num_roads(), 23);
  TEST("parsing regions from osm", objs.num_regions(), 12);
  // test binary io
  objs.write_osm_objects("./test.bin");

  volm_osm_objects objs_in("./test.bin");

  TEST("binary io of points",  objs_in.num_locs(),  objs.num_locs());
  TEST("binary io of roads",   objs_in.num_roads(), objs.num_roads());
  TEST("binary io of regions", objs_in.num_regions(), objs.num_regions());

  // write the objects into kml file
  objs_in.write_pts_to_kml("./test_pts.kml");
  objs_in.write_lines_to_kml("./test_roads.kml");
  objs_in.write_polys_to_kml("./test_regions.kml");
}

static void test_osm_object_ids()
{
  std::vector<unsigned> pt_ids, line_ids, region_ids;
  for (unsigned i = 0; i < 5; i++) {
    pt_ids.push_back(i);  line_ids.push_back(i+12);  region_ids.push_back(i+31);
  }
  volm_osm_object_ids obj_ids(pt_ids, line_ids, region_ids);
  obj_ids.add_pt(13);
  obj_ids.add_line(13);
  obj_ids.add_region(13);

  // test binary io
  obj_ids.write_osm_ids("./volm_osm_object_ids.bin");

  volm_osm_object_ids_sptr obj_sptr = new volm_osm_object_ids("./volm_osm_object_ids.bin");
  std::cout << " number of location points: " << obj_sptr->num_pts() << std::endl;
  std::vector<unsigned> pt_ids_in = obj_sptr->pt_ids();
  for (unsigned int i : pt_ids_in)
    std::cout << ' ' << i;
  std::cout << std::endl;
  std::vector<unsigned> line_ids_in = obj_sptr->line_ids();
  for (unsigned int i : line_ids_in)
    std::cout << ' ' << i;
  std::cout << std::endl;
  std::vector<unsigned> region_ids_in = obj_sptr->region_ids();
  for (unsigned int i : region_ids_in)
    std::cout << ' ' << i;
  std::cout << std::endl;
  TEST("volm_osm_object_ids io: ", obj_ids.num_pts(), pt_ids_in.size());
  TEST("volm_osm_object_ids io: ", obj_ids.num_lines(), line_ids_in.size());
  TEST("volm_osm_object_ids io: ", obj_ids.num_regions(), region_ids_in.size());
}

static void test_osm_object()
{
  // test the volm_osm loc points
  test_osm_object_point();

  // test the volm_osm road lines;
  test_osm_object_line();

  // test volm_osm_polygon
  test_osm_object_polygon();

  // test volm_osm_objects
  std::vector<volm_osm_object_point_sptr> loc_pts;
  for (unsigned i = 0; i < 10; i++) {
    volm_osm_object_point_sptr loc = new volm_osm_object_point(volm_land_layer(i, "invalid", 0, 0.0), vgl_point_2d<double>(12.31*i, 19.80*i));
    loc_pts.push_back(loc);
  }

  std::vector<volm_osm_object_line_sptr> loc_lines;
  for (unsigned i = 0; i < 5; i++) {
    std::vector<vgl_point_2d<double> > line;
    for (unsigned j = 0; j < 4; j++)
      line.emplace_back(1.2*j+i, 3.1+i+j);
    volm_osm_object_line_sptr loc_line = new volm_osm_object_line(volm_land_layer(i, "temp", 0, 0.0), line);
    loc_lines.push_back(loc_line);
  }

  std::vector<volm_osm_object_polygon_sptr> loc_polys;
  for (unsigned i = 0; i < 2; i++) {
    vgl_polygon<double> poly;
    poly.new_sheet();
    for (unsigned j = 0; j < 5; j++)
      poly.push_back(vgl_point_2d<double>(0.31*j+i, 1.2*i+j));
    volm_osm_object_polygon_sptr loc_poly = new volm_osm_object_polygon(volm_land_layer(i, "temp", 0, 0.0), poly);
    loc_polys.push_back(loc_poly);
  }

  // test the binary io
  volm_osm_objects osm_objects(loc_pts, loc_lines, loc_polys);
  osm_objects.write_osm_objects("./volm_osm_objects.bin");

  volm_osm_objects osm_objects_in("./volm_osm_objects.bin");

  std::vector<volm_osm_object_point_sptr> loc_pts_in = osm_objects_in.loc_pts();
  std::cout << " ----------- location points ----------- " << std::endl;
  for (unsigned i = 0; i < loc_pts.size(); i++) {
    std::cout << "origin --> ";  (osm_objects.loc_pts())[i]->print();
    std::cout << "loaded --> ";  loc_pts_in[i]->print();
  }
  TEST("volm_osm_objects io", (osm_objects.loc_pts()).size(), loc_pts_in.size());

  std::vector<volm_osm_object_line_sptr> loc_lines_in = osm_objects_in.loc_lines();
  std::cout << " ----------- location roads ------------ " << std::endl;
  for (unsigned i = 0; i < loc_lines_in.size(); i++) {
    std::cout << "origin --> ";  (osm_objects.loc_lines())[i]->print();
    std::cout << "loaded --> ";  loc_lines_in[i]->print();
  }
  TEST("volm_osm_objects io", (osm_objects.loc_lines()).size(), loc_lines_in.size());

  std::vector<volm_osm_object_polygon_sptr> loc_regions_in = osm_objects_in.loc_polys();
  std::cout << " ----------- location regions ------------ " << std::endl;
  for (unsigned i = 0; i < loc_regions_in.size(); i++) {
    std::cout << "origin --> ";  (osm_objects.loc_polys())[i]->print();
    std::cout << "loaded --> ";  loc_regions_in[i]->print();
  }
  TEST("volm_osm_objects io", (osm_objects.loc_polys()).size(), loc_regions_in.size());

  // test volm_osm_object_ids
  test_osm_object_ids();

  std::string osm_file = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/tests/test.osm";
  if (vul_file::exists(osm_file))
    test_load_osm(osm_file);

}

TESTMAIN(test_osm_object);
