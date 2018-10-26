#include <testlib/testlib_test.h>
#include <bkml/bkml_parser.h>
#include <bkml/bkml_write.h>
#include <vcl_where_root_dir.h>

#define EPSILON 1E-7
static bool near_eq(std::vector<vgl_point_2d<double> > const& pts1, std::vector<vgl_point_2d<double> > const& pts2)
{
  if (pts1.size() != pts2.size())
    return false;
  unsigned n_pts = pts1.size();
  bool is_equal = true;
  for (unsigned i = 0; (i < n_pts && is_equal); i++) {
    is_equal = is_equal &&
               std::fabs(pts1[i].x()-pts2[i].x()) < EPSILON &&
               std::fabs(pts1[i].y()-pts2[i].y()) < EPSILON;
  }
  return is_equal;
}

static void test_polygon_kml_io()
{
  // parse polygon from kml file
  std::string kml_file = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/bkml/tests/test_region.kml";

  vgl_polygon<double> outer;
  vgl_polygon<double> inner;
  unsigned n_out, n_in;
  vgl_polygon<double> poly = bkml_parser::parse_polygon_with_inner(kml_file, outer, inner, n_out, n_in);
  std::cout << n_out << " polygon outer boundaries are parsed:\n";
  outer.print(std::cout);
  std::cout << n_in << " polygon inner boundaries are parsed:\n";
  inner.print(std::cout);

  TEST("polygon parsing", poly.num_sheets(), n_in+n_out);
  TEST("polygon outer boundary parsing", outer.num_sheets(), n_out);
  TEST("polygon inner boundary parsing", inner.num_sheets(), n_in);

  // verify the parsed geometry
  vgl_point_2d<double>  pt_in(-71.389837, 41.831669);
  vgl_point_2d<double> pt_out(-71.385688, 41.828364);
  TEST("polygon geometry", poly.contains(pt_in),   true);
  TEST("polygon geometry", poly.contains(pt_out), false);

  // write out the polygon
  std::string out_kml = "./test_bkml_polygon.kml";
  std::vector<std::pair<vgl_polygon<double>, vgl_polygon<double> > > poly_all;
  for (unsigned i = 0; i < n_out; i++) {
    vgl_polygon<double> out_sheet(outer[i]);
    vgl_polygon<double>  in_sheet(inner[i]);
    poly_all.emplace_back(out_sheet, in_sheet);
  }
  std::cout << "There are " << poly_all.size() << " regions " << std::endl;
  std::ofstream ofs(out_kml.c_str());
  bkml_write::open_document(ofs);
  bkml_write::write_polygon(ofs, poly_all);
  bkml_write::close_document(ofs);
  ofs.close();

  // verify the kml IO
  vgl_polygon<double> outer_in;
  vgl_polygon<double> inner_in;
  unsigned n_out_in, n_in_in;
  vgl_polygon<double> poly_in = bkml_parser::parse_polygon_with_inner(out_kml, outer_in, inner_in, n_out_in, n_in_in);
  bool good = true;
  good = (n_out == n_out_in) &&(n_in == n_in_in) && (poly_in.num_sheets() == poly.num_sheets());
  TEST("p_out == p_in: number of sheet", good, true);
  for (unsigned i = 0; (i < n_out && good); i++)
    good = good && (near_eq(outer[i], outer_in[i]));
  TEST("p_out == p_in: polygon outer boundary", good, true);
  for (unsigned i = 0; (i < n_in  && good);  i++)
    good = good && (near_eq(inner[i], inner_in[i]));
  TEST("p_out == p_in: polygon inner boundary", good, true);
  return;
}

static void test_bkml()
{
  std::cout << "\n***********************************\n";
  std::cout << "Test polygon parser/writer\n";
  std::cout << "*************************************\n\n";
  test_polygon_kml_io();

  return;
}

TESTMAIN(test_bkml);
