// Some tests for vgl_sphere_3d
// Ian Scott, Aug 2005.
#include <iostream>
#include <testlib/testlib_test.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_line_3d_2_points.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_sphere()
{
  std::cout << "Simple sphere\n";

  // Default sphere
  vgl_sphere_3d<double> s;
  // Unit sphere, centered at the origin
  vgl_sphere_3d<double> u(0, 0, 0, 1.0);

  TEST("default sphere is empty",  s.is_empty(), true);
  TEST("unit sphere is not empty", u.is_empty(), false);

  TEST("origin is not inside empty sphere",s.contains( vgl_point_3d<double>(0,0,0) ), false);
  TEST("origin is inside unit sphere",     u.contains( vgl_point_3d<double>(0,0,0) ), true);
  TEST("(1,0,0) is inside unit sphere",    u.contains( vgl_point_3d<double>(1,0,0) ), true);
  TEST("(1,1,0) is outside unit sphere",   u.contains( vgl_point_3d<double>(1,1,0) ), false);

  // l1 is the X-axis
  vgl_line_3d_2_points<double> l1(vgl_point_3d<double>(-2,0,0),vgl_point_3d<double>(2,0,0));
  vgl_point_3d<double> p1, p2;
  TEST("clip x-axis to empty sphere", s.clip(l1, p1, p2), false);
  TEST("clip x-axis to unit sphere",  u.clip(l1, p1, p2), true);
  TEST("Intersection point 1", p1, vgl_point_3d<double>(-1,0,0));
  TEST("Intersection point 2", p2, vgl_point_3d<double>(1,0,0));

  // l2 is the line (y=1,z=0) parallel to the X axis, touching the unit sphere in (0,1,0)
  vgl_line_3d_2_points<double> l2(vgl_point_3d<double>(-2,1,0),vgl_point_3d<double>(2,1,0));
  TEST("clip (y=1,z=0) to unit sphere", u.clip(l2, p1, p2), true);
  TEST("Intersection point 1", p1, vgl_point_3d<double>(0,1,0));
  TEST("Intersection point 2", p2, vgl_point_3d<double>(0,1,0));

  // Test basic i/o
  std::cout << u << std::endl;
  std::ostringstream oss;
  oss << u.centre().x() << " "  << u.centre().y() << " "
      << u.centre().z() << " "  << u.radius();
  std::istringstream iss(oss.str());
  vgl_sphere_3d<double> v;
  iss >> v;
  TEST("Basic i/o", u==v, true);
}

TESTMAIN(test_sphere);
