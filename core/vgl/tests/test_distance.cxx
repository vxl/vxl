// Some tests for vgl_distance
// Amitha Perera, Sep 2001.
#include <testlib/testlib_test.h>

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>

#include <vcl_cmath.h> // for vcl_sqrt()


static void test_point_to_polygon()
{
  vgl_polygon p;
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

static void test_distance()
{
  test_point_to_polygon();
}

TESTMAIN(test_distance);
