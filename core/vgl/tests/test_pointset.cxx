// Some tests for vgl_spline_*d
// J.L. Mundy August, 2015
#include <iostream>
#include <fstream>
#include <cmath>
#include <testlib/testlib_test.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_bounding_box.h>
#include <vgl/vgl_box_3d.h>
#include <vcl_compiler.h>
#include <vpl/vpl.h>

static void test_pointset()
{
  vgl_point_3d<double>  p0(1.0, 2.0, 3.0);
  vgl_point_3d<double>  p1(4.0, 5.0, 6.0);
  vgl_point_3d<double>  p2(7.0, 8.0, 9.0);
  vgl_vector_3d<double>  n0(1.0, 0.0, 0.0);
  vgl_vector_3d<double>  n1(0.0, 1.0, 0.0);
  vgl_vector_3d<double>  n2(0.0, 0.0, 1.0);

  std::vector<vgl_point_3d<double> > pts;
  pts.push_back(p0);  pts.push_back(p1);   pts.push_back(p2);
  std::vector<vgl_vector_3d<double> > normals;
  normals.push_back(n0);  normals.push_back(n1);   normals.push_back(n2);
  vgl_pointset_3d<double> ptset(pts, normals);

  // test accessors
  bool good = ptset.npts() == 3;
  good = good && ptset.has_normals();
  good = good && (ptset.p(1)==p1) && (ptset.n(1) == n1);
  good = good && ptset.set_point(2, vgl_point_3d<double>(1.0, 1.0, 1.0));
  good = good && ptset.set_normal(2, vgl_vector_3d<double>(0.577, 0.577, 0.577));
  good = good && ptset.p(2).x() == 1.0 && ptset.n(2).x() == 0.577;
  TEST(" Accessors ", good , true);

  // test bounding box
  vgl_box_3d<double> box = vgl_bounding_box(ptset);
  vgl_point_3d<double> min_pt = box.min_point();
  vgl_point_3d<double> max_pt = box.max_point();
  good = (min_pt == vgl_point_3d<double>(1.0, 1.0, 1.0)) && (max_pt == p1);
  TEST(" bounding box ", good , true);
  // test IO
  std::string path = "./test_ptset_io.txt";
  std::ofstream ostr(path.c_str());
  ostr << ptset;
  ostr.close();
  std::ifstream istr(path.c_str());
  vgl_pointset_3d<double> io_ptset;
  istr >> io_ptset;
  good = io_ptset == ptset;
  TEST("pointset_ I/O", good, true);
  vpl_unlink(path.c_str());
}

TESTMAIN(test_pointset);
