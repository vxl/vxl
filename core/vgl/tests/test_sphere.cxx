// Some tests for vgl_sphere
// Ian Scott, Aug 2005.
#include <testlib/testlib_test.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vcl_iostream.h>

static void test_sphere()
{
  vcl_cout << "Simple sphere\n";

  // Simple triangle
  vgl_sphere_3d<double> s;

  
  TEST("default sphere is empty", s.is_empty(), true);
  
  vgl_sphere_3d<double> u(0, 0, 0, 1.0);
  TEST("unit sphere is not empty", u.is_empty(), false);

  TEST("O not inside empty sphere",s.contains( vgl_point_3d<double>(0,0,0) ), false);
  TEST("O inside unit sphere",u.contains( vgl_point_3d<double>(0,0,0) ), true);
  TEST("(1,0,0) inside unit sphere",u.contains( vgl_point_3d<double>(1,0,0) ), true);
  TEST("(1,1,0) outside unit sphere",u.contains( vgl_point_3d<double>(1,1,0) ), false);


  vgl_line_3d_2_points<double> l1(vgl_point_3d<double>(-2,0,0),vgl_point_3d<double>(2,0,0));
  vgl_point_3d<double> p1, p2;
  TEST("clip x-axis to empty sphere", s.clip(l1, p1, p2), false);
  TEST("clip x-axis to unit sphere", u.clip(l1, p1, p2), true);
  TEST("clip x-axis to unit sphere 1", p1, vgl_point_3d<double>(-1,0,0));
  TEST("clip x-axis to unit sphere 2", p2, vgl_point_3d<double>(1,0,0));

  vgl_line_3d_2_points<double> l2(vgl_point_3d<double>(-2,1,0),vgl_point_3d<double>(2,1,0));
  TEST("clip y,z=1,0 to unit sphere", u.clip(l2, p1, p2), true);
  TEST("clip y,z=1,0 to unit sphere 1", p1, vgl_point_3d<double>(0,1,0));
  TEST("clip y,z=1,0 to unit sphere 2", p2, vgl_point_3d<double>(0,1,0));

}




TESTMAIN(test_sphere);
