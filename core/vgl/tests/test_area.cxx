#include <vgl/vgl_polygon.h>
#include <vgl/vgl_area.h>

#include <testlib/testlib_test.h>

#include "test_driver.h"

void
test_simple( )
{
  {
    float cont1[] = { 0,0,  5,0,  5,3,  0,3 };
    vgl_polygon poly1 = new_polygon( cont1 );

    testlib_test_begin("rectangle (ccw) signed");
    testlib_test_perform( close( vgl_area_signed(poly1), 15 ) );

    testlib_test_begin("rectangle (ccw) unsigned");
    testlib_test_perform( close( vgl_area(poly1), 15 ) );
  }

  {
    float cont1[] = { 0,0,  0,5,  4,5,  4,0 };
    vgl_polygon poly1 = new_polygon( cont1 );

    testlib_test_begin("rectangle (cw) signed");
    testlib_test_perform( close( vgl_area_signed(poly1), -20 ) );

    testlib_test_begin("rectangle (cw) unsigned");
    testlib_test_perform( close( vgl_area(poly1), 20 ) );
  }
}

void
test_holey( )
{
  {
    float cont1[] = { 0,0,  5,0,  5,5,  0,5 };
    float cont2[] = { 1,1,  1,2,  2,2,  2,1 };
    vgl_polygon poly = new_polygon( cont1 );
    add_contour( poly, cont2 );

    testlib_test_begin("rectangle with rectgular cutout signed");
    testlib_test_perform( close( vgl_area_signed(poly), 24 ) );

    testlib_test_begin("rectangle with rectgular cutout unsigned");
    testlib_test_perform( close( vgl_area(poly), 24 ) );
  }
}


MAIN( test_area )
{
  START("area");

  test_simple();
  test_holey();

  SUMMARY();
}
