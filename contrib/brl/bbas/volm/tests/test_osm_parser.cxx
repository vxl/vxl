#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_osm_parser.h>
#include <bkml/bkml_parser.h>
#include <bkml/bkml_write.h>
#include <vcl_where_root_dir.h>

static void test_osm_parser()
{
  std::string filename = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/tests/test.osm";

  //test the method of parsing places
  std::vector<vgl_point_2d<double> > loc_pts;
  std::vector<std::vector<std::pair<std::string, std::string> > > pt_keys;
  volm_osm_parser::parse_points(loc_pts, pt_keys, filename);

  TEST("method of parsing places", loc_pts.size(), pt_keys.size());

  // test the method of parsing lines
  std::vector<std::vector<vgl_point_2d<double> > > lines;
  std::vector<std::vector<std::pair<std::string, std::string> > > line_keys;
  volm_osm_parser::parse_lines(lines, line_keys, filename);

  TEST("method of parsing lines", lines.size(), line_keys.size());

  // test the method of parsing polygons
  std::vector<vgl_polygon<double> > polygons;
  std::vector<std::vector<std::pair<std::string, std::string> > > poly_keys;
  volm_osm_parser::parse_polygons(polygons, poly_keys, filename);

  TEST("method of parsing polygons", polygons.size(), poly_keys.size());

  // write out
  std::string kml_file = "./test.kml";
  std::ofstream ofs_kml(kml_file.c_str());
  bkml_write::open_document(ofs_kml);

  // write out points of interests
  for (unsigned i = 0; i < loc_pts.size(); i++) {
    std::string name = "location";
    for (auto & j : pt_keys[i])
      if (j.first.compare("name:en") == 0)
        name = j.second;
    bkml_write::write_location(ofs_kml, loc_pts[i], name);
  }

  // write out paths
  for (unsigned i = 0; i < lines.size(); i++) {
    std::string name = "path";
    for (auto & j : line_keys[i])
      if (j.first.compare("name:en") == 0)
        name = j.second;
    bkml_write::write_path(ofs_kml, lines[i], name);
  }

  // write out polygons
  for (unsigned i = 0; i < polygons.size(); i++) {
    std::string name = "polygon";
    for (auto & j : poly_keys[i])
      if (j.first.compare("name:en") == 0)
        name = j.second;
    bkml_write::write_polygon(ofs_kml, polygons[i], name);
  }
  bkml_write::close_document(ofs_kml);
  ofs_kml.close();

}

TESTMAIN(test_osm_parser);
