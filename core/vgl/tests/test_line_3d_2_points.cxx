// Some tests for vgl_sphere
// Ian Scott, Aug 2005.
#include <vcl_iostream.h>
#include <vcl_limits.h>
#include <testlib/testlib_test.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_line_3d_2_points.h>

static void test_plane_intersection()
{

  vgl_plane_3d<double> pl1(vgl_vector_3d<double>(10,10,10), vgl_point_3d<double>(10,0,-10));

  TEST("O is a point on pl1 ",pl1.d(), 0.0);

  vgl_line_3d_2_points<double> l1(vgl_point_3d<double>(1,4,1),vgl_point_3d<double>(-1,-4,-1));
  TEST("O is a point on l1",collinear(l1, vgl_point_3d<double>(0,0,0)), true);

  vgl_point_3d<double> pt1 = intersection(l1,pl1);

  TEST_NEAR("intersection(l1,pl1) = O", vgl_distance(pt1, vgl_point_3d<double>(0,0,0)), 0.0, 1e-8);


  vgl_line_3d_2_points<double> l2(vgl_point_3d<double>(0,0,0),vgl_point_3d<double>(10,0,-10));
  TEST("O is a point on l2",collinear(l2, vgl_point_3d<double>(0,0,0)), true);
  TEST("plane_pt is a point on l2",collinear(l2, vgl_point_3d<double>(10,0,-10)), true);

  vgl_point_3d<double> pt2 = intersection(l2,pl1);

  const double inf = vcl_numeric_limits<double>::infinity();
  TEST("intersection(l2,pl1) = (inf,inf,inf)", pt2,
    vgl_point_3d<double>(inf, inf, inf));

  vgl_line_3d_2_points<double> l3(vgl_point_3d<double>(0,10,0),vgl_point_3d<double>(10,10,-10));
  TEST("O is not on l3",collinear(l3, vgl_point_3d<double>(0,0,0)), false);
  TEST("plane_norm is perpendicular to l3 direction)", dot_product(pl1.normal(), l3.direction()), 0.0);

  vgl_point_3d<double> pt3 = intersection(l3,pl1);

  TEST("intersection(l2,p1) = (inf,0,0)", pt3,
    vgl_point_3d<double>(inf, 0, 0));


}

void test_line_3d_2_points()
{
  vcl_cout << "********************************\n"
           << "  Testing vgl_line_3d_2_points\n"
           << "********************************\n\n";
  test_plane_intersection();
}


TESTMAIN(test_line_3d_2_points);
