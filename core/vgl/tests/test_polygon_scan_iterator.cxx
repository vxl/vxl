//:
// \file
// \brief  Polygon scan iterator tests.
// \author Amitha Perera
// \date   Apr 2002
//
// \verbatim
//  Modifications
//   25-SEP-2003 Michal Sofka - Adding test case for iterating polygon including its boundary.
//                              Iteration has been fixed, so conditions were corrected to test
//                              the exact number points inside the polygon (not just range).
// \endverbatim

#include <testlib/testlib_test.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>

typedef vgl_polygon::point_t       Point_type;
typedef vgl_polygon                Polygon_type;
typedef vgl_polygon_scan_iterator  Polygon_scan;

static void
test_without_boundary()
{
  Polygon_type poly;
  poly.new_sheet();
  poly.push_back( Point_type(-0.7f,-0.4f) );
  poly.push_back( Point_type( 2.3f, 2.6f) );
  poly.push_back( Point_type( 2.3f,-0.4f) );

  testlib_test_begin("iteration without boundary");
  unsigned int count=0;
  Polygon_scan scan_itr(poly, false);
  for ( scan_itr.reset(); scan_itr.next(); ) {
    count += scan_itr.endx() - scan_itr.startx() + 1;
  }
  // There are 6 points with integer-valued coordinates
  // within the polygon
  testlib_test_perform( count == 6 );
}

static void
test_with_boundary()
{
  Polygon_type poly;
  poly.new_sheet();
  poly.push_back( Point_type( 10.5f, 20.5f ) );
  poly.push_back( Point_type( 12.5f, 20.5f) );
  poly.push_back( Point_type( 12.5f, 22.5f) );
  poly.push_back( Point_type( 10.5f, 22.5f) );

  testlib_test_begin("iteration with boundary");
  unsigned int count=0;
  Polygon_scan scan_itr(poly);
  for ( scan_itr.reset(); scan_itr.next(); ) {
    count += scan_itr.endx() - scan_itr.startx() + 1;
  }
  // There are points within the polygon. This includes points with
  // integer valued coordinates and the ones at the boundary
  // (each scan line begins at the boundary of the polygon).
  testlib_test_perform( count == 16  );
}

static void
test_denegrate_polygon()
{
  {
    Polygon_type poly;
    poly.new_sheet();
    poly.push_back( Point_type( 10, 10) );
    poly.push_back( Point_type( 39, 10) );
    poly.push_back( Point_type( 39, 39) );
    poly.push_back( Point_type( 10, 10) );

    testlib_test_begin("zero-lengthed edges");
    Polygon_scan scan_itr( poly );
    unsigned int count=0;
    for ( scan_itr.reset(); scan_itr.next(); ) {
      count += scan_itr.endx() - scan_itr.startx() + 1;
    }
    // There are 465 points with integer-valued coordinates
    // within the polygon.
    testlib_test_perform( count == 465 );
  }

  {
    Polygon_type poly;

    testlib_test_begin("empty polygon");
    Polygon_scan scan_itr( poly );
    unsigned int count=0;
    for ( scan_itr.reset(); scan_itr.next(); ) {
      count += scan_itr.endx() - scan_itr.startx() + 1;
    }
    testlib_test_perform( count == 0 );
  }
}


static void test_polygon_scan_iterator()
{
  test_without_boundary();
  test_with_boundary();
  test_denegrate_polygon();
}

TESTMAIN(test_polygon_scan_iterator);
