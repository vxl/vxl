// Some tests for vgl_plane_3d
// J.L. Mundy Sept. 17, 2010

#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_3d.h>
#include <vcl_cmath.h>
static void test_constructor()
{
  double a = 0.0, b = 10.0, c = 0.0, d = 0.0;
  vgl_plane_3d<double> plane_abcd(a, b, c, d);
  vgl_vector_3d<double> n(0.0, 1.0, 0.0);
  vgl_point_3d<double> p0(0.0,0.0,0.0);
  vgl_plane_3d<double> plane_ptn(n, p0);
  vgl_vector_3d<double> nabcd = plane_abcd.normal(), nptn = plane_ptn.normal();
  double dist = vcl_fabs(1.0-dot_product(nabcd, nptn));
  TEST_NEAR("Constructors and Normal", dist ,0.0, 1e-5);
}

static void test_operations()
{
  // a plane rotated 45 degrees around the Y axis, i.e. normal is 
  // perpendicular to Y
  double a = 1.0, b = 0.0, c = 1.0, d = 0.0;
  vgl_plane_3d<double> plane(a, b, c, d);
  vgl_point_3d<double> p3d(1.0, 1.0, -1.0);
  vgl_point_2d<double> p2d, p2dy, p2dys;
  bool good = plane.planar_coords(p3d, p2d, 1e-5);
  double er = vcl_fabs(p2d.x()-vcl_sqrt(2.0));
  // a plane with the Y axis as its normal
  a = 0.0, b = 1.0, c = 0.0;
  vgl_plane_3d<double> plane1(a, b, c, d);
  vgl_point_3d<double> p3dy(1.0, 0.0, 1.0);
  good = good && plane1.planar_coords(p3dy, p2dy, 1e-5);
  er = er + vcl_fabs(p2dy.x()-1.0)+vcl_fabs(p2dy.y()-1.0);
  // shift the plane along the y axis
  d = -10.0;
  vgl_plane_3d<double> plane1s(a, b, c, d);
  vgl_point_3d<double> p3dys(1.0, 10.0, 1.0);
  good = good && plane1s.planar_coords(p3dys, p2dys, 1e-5);
  er = er + vcl_fabs(p2dys.x()-1.0)+vcl_fabs(p2dys.y()-1.0);
  good = good && (er < 1e-5);
  TEST("Planar Coordinate System", good, true);
}

void test_plane_3d()
{
  vcl_cout << "*****************************\n"
           << " Testing vgl_plane_3d\n"
           << "*****************************\n\n";

  test_constructor();
  test_operations();
}


TESTMAIN(test_plane_3d);
