// Some tests for vgl_plane_3d
// J.L. Mundy Sept. 17, 2010

#include <iostream>
#include <sstream>
#include <cmath>
#include <testlib/testlib_test.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_closest_point.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
static void test_constructor()
{
  double a = 0.0, b = 10.0, c = 0.0, d = 0.0;
  vgl_plane_3d<double> plane_abcd(a, b, c, d);
  vgl_vector_3d<double> n(0.0, 1.0, 0.0);
  vgl_point_3d<double> p0(0.0,0.0,0.0);
  vgl_plane_3d<double> plane_ptn(n, p0);
  vgl_vector_3d<double> nabcd = plane_abcd.normal(), nptn = plane_ptn.normal();
  double dist = std::fabs(1.0-dot_product(nabcd, nptn));
  TEST_NEAR("Constructors and Normal", dist ,0.0, 1e-5);
  // case I coincident
  vgl_vector_3d<double> dir0(-1.0, 0.0, 0.0), dir1(0.0, 0.0, 1.0);
  vgl_ray_3d<double> r0(p0, dir0), r1(p0, dir1);
  vgl_plane_3d<double> plane_coin_rays(r0, r1);
  vgl_vector_3d<double> ncoin = plane_coin_rays.normal();
  dist = std::fabs(1.0-dot_product(ncoin, nptn));
  TEST_NEAR("Coincident ray constructor", dist ,0.0, 1e-5);
  vgl_point_3d<double> p0p(-1.0,0.0,0.0);
  vgl_ray_3d<double> r0p(p0p, dir1);
  vgl_plane_3d<double> plane_para_rays(r0p, r1);
  vgl_vector_3d<double> npara = plane_para_rays.normal();
  dist = std::fabs(1.0-dot_product(npara, nptn));
  TEST_NEAR("Parallel ray constructor", dist ,0.0, 1e-5);
}

static void test_operations()
{
  // a plane rotated 45 degrees around the Y axis, i.e. normal is
  // perpendicular to Y
  double a = 1.0, b = 0.0, c = 1.0, d = 0.0;
  vgl_plane_3d<double> plane(a, b, c, d);
  vgl_point_3d<double> p3d(1.0, 1.0, -1.0);
  vgl_point_2d<double> p2d, p2dy, p2dys;
  bool good = plane.plane_coords(p3d, p2d, 1e-5);
  double er = std::fabs(p2d.x()-std::sqrt(2.0));

  // a plane with the Y axis as its normal
  a = 0.0, b = 1.0, c = 0.0;
  vgl_plane_3d<double> plane1(a, b, c, d);
  vgl_point_3d<double> p3dy(1.0, 0.0, 1.0);
  good = good && plane1.plane_coords(p3dy, p2dy, 1e-5);
  er +=  std::fabs(p2dy.x()-1.0)+std::fabs(p2dy.y()-1.0);
  // shift the plane along the y axis
  d = -10.0;
  vgl_plane_3d<double> plane1s(a, b, c, d);
  vgl_point_3d<double> p3dys(1.0, 10.0, 1.0);
  good = good && plane1s.plane_coords(p3dys, p2dys, 1e-5);
  er +=  std::fabs(p2dys.x()-1.0)+std::fabs(p2dys.y()-1.0);
  good = good && (er < 1e-5);
  TEST("Plane Coordinate System: world -> plane", good, true);

  vgl_point_3d<double> p3d1, p3d2, p3d3;
  p3d1 = plane.world_coords(p2d);
  p3d2 = plane1.world_coords(p2dy);
  p3d3 = plane1s.world_coords(p2dys);
  er = (p3d1-p3d).length();
  er += (p3d2-p3dy).length();
  er += (p3d3-p3dys).length();
  TEST_NEAR("Plane Coordinate System: plane -> world",er,0.0,1e-5);

  // test with a realistic plane
  vgl_point_3d<double> pt3d(-51.26, -7045.5, -50.0);
  vgl_plane_3d<double> plane_real(-.162, 0.987, 0.0, 6943.55);
  vgl_point_3d<double> pt3d_on = vgl_closest_point(pt3d, plane_real);
  vgl_point_2d<double> pt2d;
  good = plane_real.plane_coords(pt3d_on, pt2d, 1e-5);
  TEST("Realistic plane test: world -> plane", good, true);
  vgl_point_3d<double> prec_3d = plane_real.world_coords(pt2d);
  er = (prec_3d-pt3d_on).length();
  TEST_NEAR("Realistic plane test", er, 0.0, 1e-5);

  // test stream operators
  double as = 0.1, bs = 0.5, cs=0.86, ds=-1.0;
  vgl_plane_3d<double> pstr(as, bs, cs, ds);
  std::stringstream ss;
  ss << pstr;
  vgl_plane_3d<double> prec;
  ss >> prec;
  TEST("plane stream ops", pstr == prec, true);
}

void test_plane_3d()
{
  std::cout << "**********************\n"
           << " Testing vgl_plane_3d\n"
           << "**********************\n\n";

  test_constructor();
  test_operations();
}


TESTMAIN(test_plane_3d);
