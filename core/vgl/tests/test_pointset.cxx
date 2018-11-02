
// J.L. Mundy August, 2015
#include <iostream>
#include <fstream>
#include <cmath>
#include <testlib/testlib_test.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_bounding_box.h>
#include <vgl/vgl_box_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  istr.close();
  vpl_unlink(path.c_str());
  //test space separated pointset IO
  std::ofstream sostr(path.c_str());
  sostr << p0.x() << ' ' << p0.y() << ' ' << p0.z() << std::endl;
  sostr << p1.x() << ' ' << p1.y() << ' ' << p1.z() << std::endl;
  sostr << p2.x() << ' ' << p2.y() << ' ' << p2.z() << std::endl;
  sostr.close();
  vgl_pointset_3d<double> spset;
  std::ifstream sistr(path.c_str());
  sistr>>spset;
  good = spset.npts() == 3;
  good = good && spset.p(0) == p0;
  TEST("spaced pointset_ I/O", good, true);
  sistr.close();
  vpl_unlink(path.c_str());
  // test poinset with scalars
  std::vector<double > sclrs;
  sclrs.push_back(1.0);   sclrs.push_back(1.5);   sclrs.push_back(2.0);
  vgl_pointset_3d<double> ptset_sc(pts, sclrs), ptset_sc_in;
  good = ptset_sc.has_scalars() && !ptset_sc.has_normals();
  good = good && ptset_sc.sc(1) == 1.5;
  std::ofstream scostr(path.c_str());
  scostr << ptset_sc;
  scostr.close();
  std::ifstream scistr(path.c_str());
  scistr >> ptset_sc_in;
  good = good && (ptset_sc_in == ptset_sc);
  TEST("pointset with scalars", good, true);
  scistr.close();
  vpl_unlink(path.c_str());
  vgl_pointset_3d<double> ptset_nsc(pts, normals,sclrs), ptset_nsc_in;
  good = ptset_nsc.has_scalars() && ptset_nsc.has_normals();
  good = good && ptset_nsc.sc(2) == 2.0;
  std::ofstream nscostr(path.c_str());
  nscostr << ptset_nsc;
  nscostr.close();
  std::ifstream nscistr(path.c_str());
  nscistr >> ptset_nsc_in;
  good = good && (ptset_nsc_in == ptset_nsc);
  nscistr.close();
  TEST("pointset with scalars and normals", good, true);
  vpl_unlink(path.c_str());
}

TESTMAIN(test_pointset);
