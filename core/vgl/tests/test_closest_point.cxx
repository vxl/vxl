// Some tests for vgl_closest_point
// Peter Vanroose, 5 June 2003
#include <testlib/testlib_test.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vcl_iostream.h>

void testLine3DClosestPoints()
{
  vgl_homg_point_3d<double> p1, p2, p3, p4;
  vgl_homg_line_3d_2_points<double> l1;
  vgl_homg_line_3d_2_points<double> l2;
  vcl_pair<vgl_homg_point_3d<double>, vgl_homg_point_3d<double> > pts;

  // test for parallel lines
  p1.set(0,1,0);
  p2.set(1,0,0,0);
  l2.set(p1, p2);
  pts = vgl_closest_points(l1, l2);
  // result should be (1,0,0,0) for both pts
  if (pts.first!=pts.second)
    vcl_cout << "parallel test failed, points should be equal.\n"
             << "points are: " << pts.first << " " << pts.second 
             << vcl_endl;
  else if (pts.first!=p2)
    vcl_cout << "parallel test failed, points should be " << p2 << '\n'
             << "points are: " << pts.first << " " << pts.second 
             << vcl_endl;
  TEST("Parallel test", pts.first==pts.second && pts.first==p2, true);

  // test for intersecting lines.
  p1.set(0,0,0);
  p2.set(1,1,1);
  l1.set(p1,p2);
  p1.set(2,0,0);
  l2.set(p1,p2);
  pts = vgl_closest_points(l1, l2);
  // result should be (1,1,1) for both pts
  if (pts.first != pts.second)
    vcl_cout << "Intersect test failed, points should be equal\n"
             << "points are: " << pts.first << " " << pts.second 
             << vcl_endl;
  else if (pts.first != p2)
    vcl_cout << "Intersect test failed, points should be " << p2 << '\n'
             << "points are: " << pts.first << " " << pts.second 
             << vcl_endl;
  TEST("Intersect test", pts.first==pts.second && pts.first==p2, true);

  // now test for skew lines
  //
  // The lines are diagonals on neighbouring faces of a unit cube. 
  // The diagonals are chosen so they do not meet. The closest
  // distance between the two lines is 1/sqrt(3). There is actually
  // a way of visualising the problem that makes the answer obvious
  // (see visualisation course by Geoff Wyvill and Bob Parslow).

  p1.set(0,0,0);
  p2.set(1,1,0);
  l1.set(p1,p2);
  p1.set(1,0,0);
  p2.set(1,1,1);
  l2.set(p1,p2);
  pts = vgl_closest_points(l1,l2);
  vcl_cout << "Closest points are: " << pts.first << " " << pts.second << '\n';
  TEST_NEAR("Skew lines test", vgl_distance(pts.first,pts.second),1/vcl_sqrt(3.0),1e-8);
  TEST("Skew lines test", pts.first, vgl_homg_point_3d<double>(2,2,0,3));
  TEST("Skew lines test", pts.second,vgl_homg_point_3d<double>(3,1,1,3));
}

MAIN( test_closest_point )
{
  START("test closest_point");
  testLine3DClosestPoints();
  SUMMARY();
}
