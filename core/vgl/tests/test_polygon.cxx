// Some tests for vgl_polygon
// Amitha Perera, Sep 2001.
#include <testlib/testlib_test.h>
#include <vgl/vgl_polygon.h>
#include <vcl_iostream.h>

static void test_simple_polygon()
{
  vcl_cout << "Simple polygon\n";

  // Simple triangle
  vgl_polygon<double> p;
  p.new_sheet();
  p.push_back( 0.0, 0.0 );
  p.push_back( 5.0, 0.0 );
  p.push_back( 5.0, 1.0 );
  p.print(vcl_cout);

  TEST("inside",      p.contains( 2.5,  0.3 ), true );
  TEST("outside (1)", p.contains( 2.5,  0.6 ), false );
  TEST("outside (2)", p.contains( 5.1,  0.1 ), false );
  TEST("outside (3)", p.contains( 5.1,  0.0 ), false );
  TEST("outside (4)", p.contains( 2.0, -1.0 ), false );
  TEST("outside (5)", p.contains(-2.5, -0.5 ), false );
}


static void test_disjoint_polygon()
{
  vcl_cout << "Disjoint polygons\n";

  // Simple triangle
  vgl_polygon<float> p;
  p.new_sheet();
  p.push_back( 0.0f, 0.0f );
  p.push_back( 5.0f, 0.0f );
  p.push_back( 5.0f, 1.0f );
  // Another disjoint triangle
  p.new_sheet();
  p.push_back( 10.0f, 10.0f );
  p.push_back( 15.0f, 10.0f );
  p.push_back( 15.0f, 11.0f );
  p.print(vcl_cout);

  TEST("inside poly1",p.contains(  2.5f,  0.3f ), true );
  TEST("inside poly2",p.contains( 12.5f, 10.3f ), true );
  TEST("outside (1)", p.contains(  2.5f,  0.6f ), false );
  TEST("outside (2)", p.contains(  5.1f,  0.1f ), false );
  TEST("outside (3)", p.contains(  5.1f,  0.0f ), false );
  TEST("outside (4)", p.contains(  2.0f, -1.0f ), false );
  TEST("outside (5)", p.contains( -2.5f, -0.5f ), false );
  TEST("oustide (6)", p.contains( 12.5f, 10.6f ), false );
  TEST("outside (7)", p.contains( 15.1f, 10.0f ), false );
}

static void test_holey_polygon()
{
  vcl_cout << "Polygon with holes\n";

  // Simple triangle
  vgl_polygon<double> p;
  p.new_sheet();
  p.push_back( 0.0, 0.0 );
  p.push_back( 5.0, 0.0 );
  p.push_back( 5.0, 1.0 );
  // A hole
  p.new_sheet();
  p.push_back( 3.0, 0.5 );
  p.push_back( 4.0, 0.5 );
  p.push_back( 4.0, 0.1 );
  p.print(vcl_cout);

  TEST("inside",      p.contains( 2.5,  0.3 ), true );
  TEST("oustide (1)", p.contains( 2.5,  0.6 ), false );
  TEST("outside (2)", p.contains( 5.1,  0.1 ), false );
  TEST("outside (3)", p.contains( 5.1,  0.0 ), false );
  TEST("outside (4)", p.contains( 2.0, -1.0 ), false );
  TEST("outside (5)", p.contains(-2.5, -0.5 ), false );
  TEST("oustide (6)", p.contains( 3.9,  0.4 ), false );
}

static void test_polygon()
{
  test_simple_polygon();
  test_disjoint_polygon();
  test_holey_polygon();
}

TESTMAIN(test_polygon);
