// Some tests for vgl_sphere
// Ian Scott, Aug 2005.
#include <iostream>
#include <limits>
// not used? #include <vcl_compiler.h>
#include <testlib/testlib_test.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_line_3d_2_points.h>

static void test_plane_intersection()
{
  vgl_plane_3d<double> pl1(vgl_vector_3d<double>(10,10,10), vgl_point_3d<double>(10,0,-10));

  TEST("O is a point on pl1 ",pl1.d(), 0.0);

  vgl_line_3d_2_points<double> l1(vgl_point_3d<double>(1,4,1),vgl_point_3d<double>(-1,-4,-1));
  TEST("O is a point on l1",collinear(l1, vgl_point_3d<double>(0,0,0)), true);

  vgl_line_3d_2_points<double> l2(vgl_point_3d<double>(0,0,0),vgl_point_3d<double>(10,0,-10));
  TEST("O is a point on l2",collinear(l2, vgl_point_3d<double>(0,0,0)), true);
  TEST("plane_pt is a point on l2",collinear(l2, vgl_point_3d<double>(10,0,-10)), true);

  vgl_line_3d_2_points<double> l3(vgl_point_3d<double>(0,10,0),vgl_point_3d<double>(10,10,-10));
  TEST("O is not on l3",collinear(l3, vgl_point_3d<double>(0,0,0)), false);
  TEST("plane_norm is perpendicular to l3 direction)", dot_product(pl1.normal(), l3.direction()), 0.0);
}

static void test_direction_vector()
{
  vgl_point_3d<double> p1(0,0,0);
  vgl_point_3d<double> p2(1,2,3);
  vgl_line_3d_2_points<double> l1(p1, p2);
  vgl_vector_3d<double> u = p2 - p1;
  TEST("Direction vector 1", u, l1.direction());
}


static void test_parametric_point()
{
  vgl_point_3d<double> p1(0, 0, 0);
  vgl_point_3d<double> p2(1, 2, 4);
  vgl_point_3d<double> p3(0.5, 1.0, 2.0);
  vgl_line_3d_2_points<double> l1(p1, p2);
  TEST("Parametric point: t=0.0", l1.point_t(0.0), p1);
  TEST("Parametric point: t=1.0", l1.point_t(1.0), p2);
  TEST("Parametric point: t=0.5", l1.point_t(0.5), p3);
  TEST("Parametric point: t=-1.0",l1.point_t(-1.0), vgl_point_3d<double>(-1,-2,-4));
  TEST("Parametric point: t=2.0", l1.point_t(2.0), vgl_point_3d<double>(2,4,8));
}


void test_line_3d_2_points()
{
  std::cout << "********************************\n"
           << "  Testing vgl_line_3d_2_points\n"
           << "********************************\n\n";
  test_plane_intersection();

  test_direction_vector();

  test_parametric_point();
}


TESTMAIN(test_line_3d_2_points);
