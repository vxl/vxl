// Some tests for vgl_distance
// Amitha Perera, Sep 2001.
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_line_segment_2d.h>
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
  double d = vgl_distance_to_linesegment(3.,4.,0., 3.,4.,9., 0.,0.,0.);
  TEST_NEAR("vgl_distance_to_linesegment", d, 5, 1e-9);
  // line segment (0,1,4)--(4,4,4), point (8,7,4) ==> distance is 5
  d = vgl_distance_to_linesegment(0.,1.,4., 4.,4.,4., 8.,7.,4.);
  TEST_NEAR("vgl_distance_to_linesegment", d, 5, 1e-9);
  // line segment (0,3,4)--(6,3,4), point (2,0,0) ==> distance is 5
  d = vgl_distance_to_linesegment(0.,3.,4., 6.,3.,4., 2.,0.,0.);
  TEST_NEAR("vgl_distance_to_linesegment", d, 5, 1e-9);
  // Idem, with end points interchanged:
  d = vgl_distance_to_linesegment(3.,4.,9., 3.,4.,0., 0.,0.,0.);
  TEST_NEAR("vgl_distance_to_linesegment", d, 5, 1e-9);
  d = vgl_distance_to_linesegment(4.,4.,4., 0.,1.,4., 8.,7.,4.);
  TEST_NEAR("vgl_distance_to_linesegment", d, 5, 1e-9);
  d = vgl_distance_to_linesegment(6.,3.,4., 0.,3.,4., 2.,0.,0.);
  TEST_NEAR("vgl_distance_to_linesegment", d, 5, 1e-9);
}

static void test_point_to_line_segment_2d()
{
  vcl_cout << "-------------------------------------------------------\n"
           << "Testing vgl_distance(vgl_line_segment_2d, vgl_point_2d)\n"
           << "-------------------------------------------------------\n";
  vgl_point_2d<double> p(1, 0);
  vgl_point_2d<double> q(1, 1);
  vgl_line_segment_2d<double> l(p, q);
  vgl_point_2d<double> a(0, 0);
  vgl_point_2d<double> b(1, -1);
  vgl_point_2d<double> c(2, 2);

  double d_la = vgl_distance(l, a);
  double d_al = vgl_distance(a, l);
  TEST("Interchange arguments OK?", d_la==d_al, true);
  TEST("Test point a OK?", d_al==1.0, true);
  TEST("Test point b OK?", vgl_distance(b, l)==1.0, true);
  TEST_NEAR("Test point c OK?", vgl_distance(c,l), vcl_sqrt(2.0), 1e-9);
}


static void test_distance()
{
  test_point_to_polygon();
  test_point_to_3D_line_segment();
  test_point_to_line_segment_2d();
}


TESTMAIN(test_distance);
