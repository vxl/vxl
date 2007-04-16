// Some tests for vgl_line_segment_3d
// Kevin de Souza, Aug 2005.

#include <vcl_iostream.h>
// not used? #include <vcl_limits.h>
#include <testlib/testlib_test.h>
#include <vgl/vgl_line_segment_3d.h>


static void test_direction_vector()
{
  vgl_point_3d<double> p1(0,0,0);
  vgl_point_3d<double> p2(1,2,3);
  vgl_line_segment_3d<double> l1(p1, p2);
  vgl_vector_3d<double> u = p2 - p1;
  TEST("Direction vector 1", u, l1.direction());
}


static void test_parametric_point()
{
  vgl_point_3d<double> p1(0, 0, 0);
  vgl_point_3d<double> p2(1, 2, 4);
  vgl_point_3d<double> p3(0.5, 1.0, 2.0);
  vgl_line_segment_3d<double> l1(p1, p2);
  TEST("Parametric point: t=0.0", l1.point_t(0.0), p1);
  TEST("Parametric point: t=1.0", l1.point_t(1.0), p2);
  TEST("Parametric point: t=0.5", l1.point_t(0.5), p3);
  TEST("Parametric point: t=-1.0",l1.point_t(-1.0), vgl_point_3d<double>(-1,-2,-4));
  TEST("Parametric point: t=2.0", l1.point_t(2.0), vgl_point_3d<double>(2,4,8));
}


void test_line_segment_3d()
{
  vcl_cout << "*****************************\n"
           << " Testing vgl_line_segment_3d\n"
           << "*****************************\n\n";

  test_direction_vector();

  test_parametric_point();
}


TESTMAIN(test_line_segment_3d);
