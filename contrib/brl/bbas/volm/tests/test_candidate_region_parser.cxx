#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_candidate_region_parser.h>
#include <bkml/bkml_write.h>
#include <vcl_where_root_dir.h>

static void test_candidate_region_parser()
{
  std::string kml_file = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/tests/test_region.kml";

  // obtain location points
  std::vector<vgl_point_3d<double> > landmarks0 = volm_candidate_region_parser::parse_points(kml_file, "Landmarks0");
  std::vector<vgl_point_3d<double> > landmarks1 = volm_candidate_region_parser::parse_points(kml_file, "Landmarks1");
  std::vector<vgl_point_3d<double> > landmarks2 = volm_candidate_region_parser::parse_points(kml_file, "Landmarks2");
  std::vector<vgl_point_3d<double> > landmarks_none = volm_candidate_region_parser::parse_points(kml_file, "None");

  // obtain lines
  std::vector<std::vector<vgl_point_3d<double> > > lines = volm_candidate_region_parser::parse_lines(kml_file, "Region");

  // obtain polygon with name "Region"
  vgl_polygon<double> poly_outer_region = volm_candidate_region_parser::parse_polygon(kml_file, "Region");

  // obtain polygon with name "Overhead Overlay"
  vgl_polygon<double> poly_outer_overhead = volm_candidate_region_parser::parse_polygon(kml_file, "Overhead Overlay");

  // obtain polygon with inner boundary
  vgl_polygon<double> outer;
  vgl_polygon<double> inner;
  unsigned n_out, n_in;
  vgl_polygon<double> poly_all_region = volm_candidate_region_parser::parse_polygon_with_inner(kml_file, "Region", outer, inner, n_out, n_in);

  // print out the point coordinates
  std::cout << "Landmarks0:\n";
  for (unsigned i = 0; i < landmarks0.size(); i++)
    std::cout << '\t' << i << ": " << landmarks0[i] << '\n';
  std::cout << "Landmarks1:\n";
  for (unsigned i = 0; i < landmarks1.size(); i++)
    std::cout << '\t' << i << ": " << landmarks1[i] << '\n';
  std::cout << "Landmarks2:\n";
  for (unsigned i = 0; i < landmarks2.size(); i++)
    std::cout << '\t' << i << ": " << landmarks2[i] << '\n';

  // print out the line
  std::cout << "Line:\n\t";
  for (auto & line : lines) {
    for (unsigned k = 0; k < line.size(); k++) {
      std::cout << line[k] << ' ';
    }
    std::cout << "\n\t";
  }
  std::cout << '\n';

  // print out the polygon
  std::cout << "Region outer boundary:\n";
  poly_outer_region.print(std::cout);
  std::cout << "Overhead Overlay outer boundary:\n";
  poly_outer_overhead.print(std::cout);
  std::cout << "Region inner boundary:\n";
  inner.print(std::cout);

  std::cout << "Entire region\n";
  poly_all_region.print(std::cout);

  TEST("parse point with name \"Landmarks0\"", landmarks0.size(), 3);
  TEST("parse point with name \"Landmarks1\"", landmarks1.size(), 4);
  TEST("parse point with name \"Landmarks0\"", landmarks2.size(), 3);
  TEST("parse point with name \"None\"",  landmarks_none.size(), 0);

  bool success = (lines.size() == 1) && (lines[0].size() == 6);
  TEST("parse line with name \"Region\"", success, true);
  TEST("parse polygon with name \"Region\"", poly_outer_region.num_sheets(), 2);
  TEST("parse polygon with name \"Overhead Overlay\"", poly_outer_overhead.num_sheets(), 3);

  success = (outer.num_sheets() == n_out) && (inner.num_sheets() == n_in);
  success &= (poly_all_region.num_sheets() == (n_in + n_out));
  TEST("parse polygon with inner boundary", success, true);
  return;
}

TESTMAIN(test_candidate_region_parser);
