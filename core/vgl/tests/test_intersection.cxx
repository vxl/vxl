// Some tests for vgl_intersection
// Gamze Tunali, Jan 2007.
#include <vcl_iostream.h>
#include <vcl_limits.h>
#include <testlib/testlib_test.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_line_3d_2_points.h>


static void test_plane_intersection()
{
  vgl_plane_3d<double> pl1(vgl_vector_3d<double>(10,10,10), vgl_point_3d<double>(10,0,-10));
  vgl_line_3d_2_points<double> l1(vgl_point_3d<double>(1,4,1),vgl_point_3d<double>(-1,-4,-1));
  vgl_point_3d<double> pt1 = vgl_intersection(l1,pl1);
  TEST_NEAR("vgl_intersection(l1,pl1) = O", vgl_distance(pt1, vgl_point_3d<double>(0,0,0)), 0.0, 1e-8);

  vgl_line_3d_2_points<double> l2(vgl_point_3d<double>(0,0,0),vgl_point_3d<double>(10,0,-10));
  vgl_point_3d<double> pt2 = vgl_intersection(l2,pl1);

  const double inf = vcl_numeric_limits<double>::infinity();
  TEST("vgl_intersection(l2,pl1) = (inf,inf,inf)", pt2,
    vgl_point_3d<double>(inf, inf, inf));

  vgl_line_3d_2_points<double> l3(vgl_point_3d<double>(0,10,0),vgl_point_3d<double>(10,10,-10));
  vgl_point_3d<double> pt3 = vgl_intersection(l3,pl1);
  TEST("vgl_intersection(l2,p1) = (inf,0,0)", pt3, vgl_point_3d<double>(inf, 0, 0));
}

static void test_three_planes()
{
  vgl_plane_3d<double> pl4(1,0,0,0), pl5(0,1,0,0), pl6(0,0,1,0);
  {
   vgl_point_3d<double> pi(pl4,pl5,pl6); // intersection
   TEST("intersection", pi, vgl_point_3d<double>(0,0,0));
   TEST("is_ideal", is_ideal(pi), false);
   vgl_point_3d<double> pj = vgl_intersection(pl4,pl5,pl6);
   TEST("three planes intersecting", pj, pi);
  }
}

void test_intersection()
{
  vcl_cout << "**************************\n"
           << " Testing vgl_intersection\n"
           << "**************************\n\n";
  test_plane_intersection();
  test_three_planes();
}

TESTMAIN(test_intersection);
