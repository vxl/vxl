//:
// \file
// \author Joseph Mundy
// \date  July 14, 2004

#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_convex_hull_2d.h>

static void test_4_point_hull()
{
  vgl_point_2d<double> p0(0.0,0.0), p1(2.0,0.0);
  vgl_point_2d<double> p2(1.0,2.0), p3(1.0,1.0);
  vgl_point_2d<double> p4(0.5,0.25), p5(0.4,0.7);
  vcl_vector<vgl_point_2d<double> > points;
  points.push_back(p0);   points.push_back(p1);
  points.push_back(p2);   points.push_back(p3);
  points.push_back(p4);   points.push_back(p5);
  vgl_convex_hull_2d<double> ch(points);
  vgl_polygon<double> poly = ch.hull();
  vcl_cout << poly << '\n';
  // A convex hull should be convex, and consequently only have a single sheet:
  TEST("Hull is convex", poly.num_sheets(), 1);
  // Points p3, p4 and p5 should lie inside the other 3:
  TEST("Hull shape is triangular", poly.num_vertices(), 3);
  TEST("Hull point 1", poly[0][0], p0);
  TEST("Hull point 2", poly[0][1], p1);
  TEST("Hull point 3", poly[0][2], p2);
  // Verify that the 3 other points are inside the convex hull:
  TEST("All points are inside the convex hull",
       poly.contains(p3) && poly.contains(p4) && poly.contains(p5), true);
}

static void test_convex_hull_2d()
{
  test_4_point_hull();
}

TESTMAIN(test_convex_hull_2d);
