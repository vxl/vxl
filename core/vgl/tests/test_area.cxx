#include <vgl/vgl_polygon.h>
#include <vgl/vgl_area.h>

#include <testlib/testlib_test.h>

#include "test_driver.h"

static void test_simple()
{
  {
    float cont1[] = { 0,0,  5,0,  5,3,  0,3 };
    vgl_polygon poly1 = new_polygon(cont1);
    TEST("rectangle (ccw) signed", close(vgl_area_signed(poly1), 15), true);
    TEST("rectangle (ccw) unsigned", close(vgl_area(poly1), 15), true);
  }

  {
    float cont1[] = { 0,0,  0,5,  4,5,  4,0 };
    vgl_polygon poly1 = new_polygon(cont1);
    TEST("rectangle (cw) signed", close(vgl_area_signed(poly1), -20), true);
    TEST("rectangle (cw) unsigned", close(vgl_area(poly1), 20), true);
  }
}

static void test_holey()
{
  float cont1[] = { 0,0,  5,0,  5,5,  0,5 };
  float cont2[] = { 1,1,  1,2,  2,2,  2,1 };
  vgl_polygon poly = new_polygon(cont1);
  add_contour(poly, cont2);
  TEST("rectangle with rectgular cutout signed", close(vgl_area_signed(poly), 24), true);
  TEST("rectangle with rectgular cutout unsigned", close(vgl_area(poly), 24), true);
}


static void test_area()
{
  test_simple();
  test_holey();
}

TESTMAIN(test_area);
