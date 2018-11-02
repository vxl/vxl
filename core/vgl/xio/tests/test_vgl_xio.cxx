#include <sstream>
#include <vgl/xio/vgl_xio_box_3d.h>
#include <vgl/xio/vgl_xio_point_2d.h>
#include <vgl/xio/vgl_xio_point_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>

#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_xio_box_3d()
{
  std::stringstream s;

  vgl_box_3d<double> b3;
  x_write(s, b3);
  TEST("XML I/O for box_3d<double>", s.str(),
       "<vgl_box_3d min_x=\"1.000000\" min_y=\"1.000000\" min_z=\"1.000000\" max_x=\"0.000000\" max_y=\"0.000000\" max_z=\"0.000000\">\n</vgl_box_3d>\n");
}

static void test_xio_point_2d()
{
  std::stringstream s;

  vgl_point_2d<double> p2(0.0,0.0);
  x_write(s, p2);
  TEST("XML I/O for vgl_point_2d<double>", s.str(),
       "<vgl_point_2d x=\"0.000000\" y=\"0.000000\">\n</vgl_point_2d>\n");
}

static void test_xio_point_3d()
{
  std::stringstream s;

  vgl_point_3d<double> p3(0.0,0.0,0.0);
  x_write(s, p3);
  TEST("XML I/O for vgl_point_3d<double>", s.str(),
       "<vgl_point_3d x=\"0.000000\" y=\"0.000000\" z=\"0.000000\">\n</vgl_point_3d>\n");
}

static void test_xio_vector_3d()
{
  std::stringstream s;

  vgl_vector_3d<double> v3;
  x_write(s, v3);
  TEST("XML I/O for vgl_vector_3d<double>", s.str(),
       "<vgl_vector_3d x=\"0.000000\" y=\"0.000000\" z=\"0.000000\">\n</vgl_vector_3d>\n");
}

static void test_vgl_xio()
{
  test_xio_box_3d();
  test_xio_point_2d();
  test_xio_point_3d();
  test_xio_vector_3d();
}

TESTMAIN(test_vgl_xio);
