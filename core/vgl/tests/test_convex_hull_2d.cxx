//:
// \file
// \author Joseph Mundy
// \date  July 14, 2004

#include <testlib/testlib_test.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_convex_hull_2d.h>

static void test_4_point_hull()
{
  vgl_point_2d<double> p0(0.0,0.0), p1(2.0,0.0);
  vgl_point_2d<double> p2(1.0,2.0), p3(1.0,1.0);
  vgl_point_2d<double> p4(0.5,0.25), p5(0.75,0.1);
  vcl_vector<vgl_point_2d<double> > points;
  points.push_back(p0);   points.push_back(p1);
  points.push_back(p2);   points.push_back(p3);
  vgl_convex_hull_2d<double> ch(points);
  vgl_polygon<double> poly = ch.hull();
  vcl_cout << poly << '\n';
  TEST("Hull Points", poly.num_vertices(), 4);
}
static void test_convex_hull_2d()
{
  test_4_point_hull();
}

TESTMAIN(test_convex_hull_2d);
