// Some tests for vgl_closest_point
// Peter Vanroose, 5 June 2003
#include <testlib/testlib_test.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h> // for sqrt()

static void testHomgLine2DClosestPoint()
{
  vgl_homg_point_2d<double> p, q;
  vgl_homg_line_2d<double> l;

  // test for coincident
  l.set(0,1,-1); q.set(0,1); p = vgl_closest_point(l,q);
  TEST("2D coincident test", p, q);
  TEST("2D coincident test", vgl_closest_point(q,l), q);
  TEST_NEAR("Distance test", vgl_distance(l,q), 0.0, 1e-8);
  TEST_NEAR("Distance test", vgl_distance(q,l), 0.0, 1e-8);
  q.set(5,1); p = vgl_closest_point(l,q);
  TEST("2D coincident test", p, q);

  // test for non-coincident
  q.set(0,2); p.set(0,1);
  TEST("2D non-coincident test", vgl_closest_point(l,q), p);
  TEST_NEAR("Distance test", vgl_distance(l,q), 1.0, 1e-8);
  q.set(5,2); p.set(5,1);
  TEST("2D non-coincident test", vgl_closest_point(q,l), p);
  TEST_NEAR("Distance test", vgl_distance(q,l), 1.0, 1e-8);
}

// Test for closest points on two 3D lines, by Brendan McCane
static void testHomgLine3DClosestPoints()
{
  vgl_homg_point_3d<double> p1, p2;
  vgl_homg_line_3d_2_points<double> l1;
  vgl_homg_line_3d_2_points<double> l2;
  vcl_pair<vgl_homg_point_3d<double>, vgl_homg_point_3d<double> > pts;

  // test for parallel lines
  p1.set(0,1,0); p2.set(1,0,0,0); l1.set(p1, p2);
  p1.set(1,4,4); p2.set(1,0,0,0); l2.set(p1, p2);
  pts = vgl_closest_points(l1, l2);
  // result should be (1,0,0,0) for both pts
  if (pts.first!=pts.second)
    vcl_cout << "parallel test failed, points should be equal.\n"
             << "points are: " << pts.first << ' ' << pts.second
             << vcl_endl;
  else if (pts.first!=p2)
    vcl_cout << "parallel test failed, points should be " << p2 << '\n'
             << "points are: " << pts.first << ' ' << pts.second
             << vcl_endl;
  TEST("Parallel test", pts.first==pts.second && pts.first==p2, true);
  TEST_NEAR("Parallel distance test", vgl_distance(l1,l2), 5.0, 1e-8);
  TEST_NEAR("Parallel distance test", vgl_distance(l1,p1), 5.0, 1e-8);
  p1.set(0,1,0);
  TEST_NEAR("Parallel distance test", vgl_distance(p1,l2), 5.0, 1e-8);

  // test for intersecting lines.
  p1.set(0,0,0); p2.set(1,1,1); l1.set(p1,p2);
  p1.set(2,0,0); p2.set(1,1,1); l2.set(p1,p2);
  pts = vgl_closest_points(l1, l2);
  // result should be (1,1,1) for both pts
  if (pts.first != pts.second)
    vcl_cout << "Intersect test failed, points should be equal\n"
             << "points are: " << pts.first << ' ' << pts.second
             << vcl_endl;
  else if (pts.first != p2)
    vcl_cout << "Intersect test failed, points should be " << p2 << '\n'
             << "points are: " << pts.first << ' ' << pts.second
             << vcl_endl;
  TEST("Intersect test", pts.first==pts.second && pts.first==p2, true);
  TEST_NEAR("Intersect distance test", vgl_distance(l1,l2), 0.0, 1e-8);

  // now test for skew lines
  //
  // The lines are diagonals on neighbouring faces of a unit cube.
  // The diagonals are chosen so they do not meet. The closest
  // distance between the two lines is 1/sqrt(3). There is actually
  // a way of visualising the problem that makes the answer obvious
  // (see visualisation course by Geoff Wyvill and Bob Parslow).

  p1.set(0,0,0); p2.set(1,1,0); l1.set(p1,p2);
  p1.set(1,0,0); p2.set(1,1,1); l2.set(p1,p2);
  pts = vgl_closest_points(l1,l2);
  vcl_cout << "Closest points are: " << pts.first << ' ' << pts.second << '\n';
  TEST("Skew lines test", pts.first, vgl_homg_point_3d<double>(2,2,0,3));
  TEST("Skew lines test", pts.second,vgl_homg_point_3d<double>(3,1,1,3));
  TEST_NEAR("Skew lines distance test", vgl_distance(pts.first,pts.second),1/vcl_sqrt(3.0),1e-8);
  TEST_NEAR("Skew lines distance test", vgl_distance(l1,pts.second),1/vcl_sqrt(3.0),1e-8);
  TEST_NEAR("Skew lines distance test", vgl_distance(pts.first,l2),1/vcl_sqrt(3.0),1e-8);
}

static void testHomgPlane3DClosestPoint()
{
  vgl_homg_point_3d<double> p, q;
  vgl_homg_plane_3d<double> l;

  // test for coincident
  l.set(0,1,0,-1); q.set(0,1,2); p = vgl_closest_point(l,q);
  TEST("3D coincident test", p, q);
  TEST("3D coincident test", vgl_closest_point(q,l), q);
  TEST_NEAR("Distance test", vgl_distance(l,q), 0.0, 1e-8);
  TEST_NEAR("Distance test", vgl_distance(q,l), 0.0, 1e-8);
  q.set(5,1,3); p = vgl_closest_point(l,q);
  TEST("3D coincident test", p, q);

  // test for non-coincident
  q.set(0,2,3); p.set(0,1,3);
  TEST("3D non-coincident test", vgl_closest_point(l,q), p);
  TEST_NEAR("Distance test", vgl_distance(l,q), 1.0, 1e-8);
  q.set(5,2,3); p.set(5,1,3);
  TEST("3D non-coincident test", vgl_closest_point(q,l), p);
  TEST_NEAR("Distance test", vgl_distance(q,l), 1.0, 1e-8);
}

static void testLine2DClosestPoint()
{
  vgl_point_2d<double> p, q;
  vgl_line_2d<double> l;

  // test for coincident
  l.set(0,1,-1); q.set(0,1); p = vgl_closest_point(l,q);
  TEST("2D coincident test", p, q);
  TEST("2D coincident test", vgl_closest_point(q,l), q);
  TEST_NEAR("Distance test", vgl_distance(l,q), 0.0, 1e-8);
  TEST_NEAR("Distance test", vgl_distance(q,l), 0.0, 1e-8);
  q.set(5,1); p = vgl_closest_point(l,q);
  TEST("2D coincident test", p, q);

  // test for non-coincident
  q.set(0,2); p.set(0,1);
  TEST("2D non-coincident test", vgl_closest_point(l,q), p);
  TEST_NEAR("Distance test", vgl_distance(l,q), 1.0, 1e-8);
  q.set(5,2); p.set(5,1);
  TEST("2D non-coincident test", vgl_closest_point(q,l), p);
  TEST_NEAR("Distance test", vgl_distance(q,l), 1.0, 1e-8);
}

static void testPlane3DClosestPoint()
{
  vgl_point_3d<double> p, q;
  vgl_plane_3d<double> l;

  // test for coincident
  l.set(0,1,0,-1); q.set(0,1,2); p = vgl_closest_point(l,q);
  TEST("3D coincident test", p, q);
  TEST("3D coincident test", vgl_closest_point(q,l), q);
  TEST_NEAR("Distance test", vgl_distance(l,q), 0.0, 1e-8);
  TEST_NEAR("Distance test", vgl_distance(q,l), 0.0, 1e-8);
  q.set(5,1,3); p = vgl_closest_point(l,q);
  TEST("3D coincident test", p, q);

  // test for non-coincident
  q.set(0,2,3); p.set(0,1,3);
  TEST("3D non-coincident test", vgl_closest_point(l,q), p);
  TEST_NEAR("Distance test", vgl_distance(l,q), 1.0, 1e-8);
  q.set(5,2,3); p.set(5,1,3);
  TEST("3D non-coincident test", vgl_closest_point(q,l), p);
  TEST_NEAR("Distance test", vgl_distance(q,l), 1.0, 1e-8);
}

static void test_closest_point()
{
  testHomgLine2DClosestPoint();
  testHomgLine3DClosestPoints();
  testHomgPlane3DClosestPoint();
  testLine2DClosestPoint();
  testPlane3DClosestPoint();
}

TESTMAIN(test_closest_point);
