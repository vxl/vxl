// Some tests for vgl_polygon
// Amitha Perera, Sep 2001.
#include <vgl/vgl_test.h>
#include <vgl/vgl_polygon.h>
#include <vcl_iostream.h>


void test_simple_polygon()
{
  vcl_cout << "Simple polygon" << vcl_endl;

  // Simple triangle
  vgl_polygon p;
  p.new_sheet();
  p.push_back( 0.0, 0.0 );
  p.push_back( 5.0, 0.0 );
  p.push_back( 5.0, 1.0 );

  TEST("inside", p.contains(  2.5,  0.3 ), true );
  TEST("oustide (1)", p.contains(  2.5,  0.6 ), false );
  TEST("outside (2)", p.contains(  5.1,  0.1 ), false );
  TEST("outside (3)", p.contains(  5.1,  0.0 ), false );
  TEST("outside (4)", p.contains(  2.0, -1.0 ), false );
  TEST("outside (5)", p.contains( -2.5, -0.5 ), false );
}


void test_disjoint_polygon()
{
  vcl_cout << "Disjoint polygons" << vcl_endl;

  // Simple triangle
  vgl_polygon p;
  p.new_sheet();
  p.push_back( 0.0, 0.0 );
  p.push_back( 5.0, 0.0 );
  p.push_back( 5.0, 1.0 );

  // Another disjoint triangle
  p.new_sheet();
  p.push_back( 10.0, 10.0 );
  p.push_back( 15.0, 10.0 );
  p.push_back( 15.0, 11.0 );

  TEST("inside poly 1", p.contains(  2.5,  0.3 ), true );
  TEST("inside poly 2", p.contains( 12.5, 10.3 ), true );
  TEST("oustide (1)", p.contains(  2.5,  0.6 ), false );
  TEST("outside (2)", p.contains(  5.1,  0.1 ), false );
  TEST("outside (3)", p.contains(  5.1,  0.0 ), false );
  TEST("outside (4)", p.contains(  2.0, -1.0 ), false );
  TEST("outside (5)", p.contains( -2.5, -0.5 ), false );
  TEST("oustide (6)", p.contains( 12.5, 10.6 ), false );
  TEST("outside (7)", p.contains( 15.1, 10.0 ), false );
}

void test_holey_polygon()
{
  vcl_cout << "Polygon with holes" << vcl_endl;

  // Simple triangle
  vgl_polygon p;
  p.new_sheet();
  p.push_back( 0.0, 0.0 );
  p.push_back( 5.0, 0.0 );
  p.push_back( 5.0, 1.0 );

  // A hole
  p.new_sheet();
  p.push_back( 3.0, 0.5 );
  p.push_back( 4.0, 0.5 );
  p.push_back( 4.0, 0.1 );

  TEST("inside", p.contains(  2.5,  0.3 ), true );
  TEST("oustide (1)", p.contains(  2.5,  0.6 ), false );
  TEST("outside (2)", p.contains(  5.1,  0.1 ), false );
  TEST("outside (3)", p.contains(  5.1,  0.0 ), false );
  TEST("outside (4)", p.contains(  2.0, -1.0 ), false );
  TEST("outside (5)", p.contains( -2.5, -0.5 ), false );
  TEST("oustide (6)", p.contains(  3.9,  0.4 ), false );
}



int
main()
{
  vgl_test_start("vgl_polygon");
  test_simple_polygon();
  test_disjoint_polygon();
  test_holey_polygon();
  return vgl_test_summary();
}
