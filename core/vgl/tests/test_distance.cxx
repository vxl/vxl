// Some tests for vgl_distance
// Amitha Perera, Sep 2001.
#include <testlib/testlib_test.h>

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>

#include <vcl_cmath.h> // for vcl_sqrt()


static void test_point_to_polygon()
{
  vgl_polygon<float> p;
  p.new_sheet();
  p.push_back( 0.0f, 0.0f );
  p.push_back( 5.0f, 0.0f );
  p.push_back( 5.0f, 5.0f );

  testlib_test_begin( "point to polygon 1" );
  testlib_test_assert_near( "distance",
                            vgl_distance( p, vgl_point_2d<float>( 6.0f, 1.0f ) ),
                            1.0f, 1e-5 );
  testlib_test_begin( "point to polygon 2" );
  testlib_test_assert_near( "distance",
                            vgl_distance( p, vgl_point_2d<float>( 6.0f, 6.0f ) ),
                            vcl_sqrt(2.0f), 1e-5 );

  testlib_test_begin( "point to polygon 3" );
  testlib_test_assert_near( "distance",
                            vgl_distance( p, vgl_point_2d<float>( 3.0f, 4.0f ) ),
                            vcl_sqrt(0.5f), 1e-5 );

  testlib_test_begin( "point to polygon argument swap" );
  testlib_test_perform( vgl_distance( p, vgl_point_2d<float>( 3.0f, 4.0f ) ) ==
                        vgl_distance( vgl_point_2d<float>( 3.0f, 4.0f ), p ) );
}

static void test_point_to_3D_line_segment()
{
  // line segment (3,4,0)--(3,4,9), point (0,0,0) ==> distance is 5
  double d = vgl_distance_to_linesegment(3,4,0, 3,4,9, 0,0,0);
  TEST_NEAR("vgl_distance_to_linesegment", d, 5, 1e-9);
  // line segment (0,1,4)--(4,4,4), point (8,7,4) ==> distance is 5
  d = vgl_distance_to_linesegment(0,1,4, 4,4,4, 8,7,4);
  TEST_NEAR("vgl_distance_to_linesegment", d, 5, 1e-9);
  // line segment (0,3,4)--(6,3,4), point (2,0,0) ==> distance is 5
  d = vgl_distance_to_linesegment(0,3,4, 6,3,4, 2,0,0);
  TEST_NEAR("vgl_distance_to_linesegment", d, 5, 1e-9);
  // Idem, with end points interchanged:
  d = vgl_distance_to_linesegment(3,4,9, 3,4,0, 0,0,0);
  TEST_NEAR("vgl_distance_to_linesegment", d, 5, 1e-9);
  d = vgl_distance_to_linesegment(4,4,4, 0,1,4, 8,7,4);
  TEST_NEAR("vgl_distance_to_linesegment", d, 5, 1e-9);
  d = vgl_distance_to_linesegment(6,3,4, 0,3,4, 2,0,0);
  TEST_NEAR("vgl_distance_to_linesegment", d, 5, 1e-9);
}

static void test_distance()
{
  test_point_to_polygon();
  test_point_to_3D_line_segment();
}

TESTMAIN(test_distance);
