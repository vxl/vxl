// Some tests for vgl_line_segment_3d
// J.L. Mundy July 2009

#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <vgl/vgl_infinite_line_3d.h>


static void test_constructor()
{
  vgl_vector_3d<double> t(0,0,2);
  vgl_point_3d<double> p(1,2,3);
  vgl_infinite_line_3d<double> inf_l(p, t);
  vgl_vector_2d<double> x0 = inf_l.x0();
  vgl_vector_3d<double> dir = inf_l.direction();
  TEST_NEAR("Constructor from point and dir - compare x0", x0.x()+x0.y() , 3.0, 1e-5);
  TEST_NEAR("Constructor from point and dir - compare dir", dir.z_ ,1.0, 1e-5);
  vgl_point_3d<double> p1(1,2,4);
  vgl_infinite_line_3d<double> inf_l1(p, p1);
  x0 = inf_l1.x0();
  dir = inf_l1.direction();
  TEST_NEAR("Constructor from point-point - compare x0", x0.x()+x0.y() , 3.0, 1e-5);
  TEST_NEAR("Constructor from point-point - compare dir", dir.z_ ,1.0, 1e-5);

}

static void test_operations()
{
  vgl_vector_3d<double> t(0,0,1);
  vgl_point_3d<double> p(1,2,3), p0, pt;
  vgl_infinite_line_3d<double> inf_l(p, t);
  p0 = inf_l.point();
  TEST("Closest point to origin", p0==vgl_point_3d<double>(1,2,0), true);
  pt = inf_l.point_t(1.0);
  TEST("Parametric point ", pt==vgl_point_3d<double>(1,2,1), true);
  vgl_point_3d<double> x(1.0, 2.0, -3.0);
  bool con = inf_l.contains(x);
  TEST("Contains ", con, true);
}

void test_infinite_line_3d()
{
  vcl_cout << "*****************************\n"
           << " Testing vgl_infinite_line_3d\n"
           << "*****************************\n\n";

  test_constructor();
  test_operations();
}


TESTMAIN(test_infinite_line_3d);
