#include <vgl/vgl_polygon.h>
#include <vgl/vgl_area.h>

#include <testlib/testlib_test.h>

static void test_simple()
{
  {
    double cont1[] = { 0,0,  5,0,  5,3,  0,3 };
    vgl_polygon<double> poly1(cont1, 4);
    TEST_NEAR("rectangle (ccw) signed", vgl_area_signed(poly1), 15, 1e-6);
    TEST_NEAR("rectangle (ccw) unsigned", vgl_area(poly1), 15, 1e-6);
  }

  {
    float cont1[] = { 0,0,  0,5,  4,5,  4,0 };
    vgl_polygon<float> poly1(cont1, 4);
    TEST_NEAR("rectangle (cw) signed", vgl_area_signed(poly1), -20, 1e-6);
    TEST_NEAR("rectangle (cw) unsigned", vgl_area(poly1), 20, 1e-6);
  }
}

static void test_holey()
{
  double cont1[] = { 0,0,  5,0,  5,5,  0,5 };
  double cont2[] = { 1,1,  1,2,  2,2,  2,1 };
  vgl_polygon<double> poly(cont1, 4);
  poly.add_contour(cont2, 4);
  TEST_NEAR("rectangle with rectgular cutout signed", vgl_area_signed(poly), 24, 1e-6);
  TEST_NEAR("rectangle with rectgular cutout unsigned", vgl_area(poly), 24, 1e-6);
}


static void test_area()
{
  test_simple();
  test_holey();
}

TESTMAIN(test_area);
