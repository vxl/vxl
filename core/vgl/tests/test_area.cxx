#include <vgl/vgl_polygon.h>
#include <vgl/vgl_area.h>

#include <testlib/testlib_test.h>

static void test_simple1()
{
  double cont1[] = { 0,0,  5,0,  5,3,  0,3 };
  vgl_polygon<double> poly1(cont1, 4);
  TEST_NEAR("rectangle (ccw) signed", vgl_area_signed(poly1), 15, 1e-6);
  TEST_NEAR("rectangle (ccw) unsigned", vgl_area(poly1), 15, 1e-6);
  vgl_point_2d<double> c = vgl_centroid(poly1);
  TEST_NEAR("rectangle (ccw) centroid x", c.x(), 2.5, 1e-6);
  TEST_NEAR("rectangle (ccw) centroid y", c.y(), 1.5, 1e-6);
}

static void test_simple2()
{
  float cont1[] = { 0,0,  0,5,  4,5,  4,0 };
  vgl_polygon<float> poly1(cont1, 4);
  TEST_NEAR("rectangle (cw) signed", vgl_area_signed(poly1), -20, 1e-6);
  TEST_NEAR("rectangle (cw) unsigned", vgl_area(poly1), 20, 1e-6);
  vgl_point_2d<float> c = vgl_centroid(poly1);
  TEST_NEAR("rectangle (cw) centroid x", c.x(), 2.0, 1e-6);
  TEST_NEAR("rectangle (cw) centroid y", c.y(), 2.5, 1e-6);
}

static void test_simple3()
{
  const double x[4]= { 1.0, 20.0, 20.0, 1.0};
  const double y[4]= { 1.0, 1.0, 20.0, 20.0};
  constexpr unsigned int size = 4;

  vgl_polygon<double> poly;
  poly.new_sheet();
  poly[0].resize(size);
  for (unsigned int ii=0; ii<size; ++ii) {
    poly[0][ii].set(x[ii],y[ii]);
  }

  TEST("rectangle (ccw)", vgl_area(poly), 19*19);
  TEST("rectangle (ccw) signed", vgl_area_signed(poly), 19*19);
}

static void test_holey()
{
  double cont1[] = { 0,0,  5,0,  5,5,  0,5 };
  double cont2[] = { 1,1,  1,2,  2,2,  2,1 };
  vgl_polygon<double> poly(cont1, 4);
  poly.add_contour(cont2, 4);
  TEST_NEAR("rectangle with rectgular cutout signed", vgl_area_signed(poly), 24, 1e-6);
  TEST_NEAR("rectangle with rectgular cutout unsigned", vgl_area(poly), 24, 1e-6);
  vgl_point_2d<double> c = vgl_centroid(poly);
  TEST_NEAR("rectangle with rectgular cutout centroid x", c.x(), 61.0/24.0, 1e-6);
  TEST_NEAR("rectangle with rectgular cutout centroid y", c.y(), 61.0/24.0, 1e-6);
}


static void test_area()
{
  test_simple1();
  test_simple2();
  test_simple3();
  test_holey();
}

TESTMAIN(test_area);
