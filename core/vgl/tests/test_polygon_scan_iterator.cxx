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
//   30-APR-2004 Peter Vanroose - Test case added for which the iterator fails
//                                (it produced a very large value for xend()),
//                                as signalled by Kenneth Fritzsche and Gehua Yang.
// \endverbatim

#include <testlib/testlib_test.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vcl_iostream.h>

typedef vgl_polygon<float>::point_t        Point_type;
typedef vgl_polygon<float>                 Polygon_type;
typedef vgl_polygon_scan_iterator<float>   Polygon_scan;

static void
test_without_boundary()
{
  Polygon_type poly;
  poly.new_sheet();
  // This is a triangle with 2 sides of length 3 parallel to the axes,
  // at X=2.3 and Y=-0.4 resp.
  poly.push_back( Point_type(-0.7f,-0.4f) );
  poly.push_back( Point_type( 2.3f, 2.6f) );
  poly.push_back( Point_type( 2.3f,-0.4f) );

  // There will be scan lines at y=0, 1 and 2.
  // Since boundary pixels (with their center outside of the triangle) are not
  // to be included, the 6 pixels are (0,0) (1,0) (2,0) (1,1) (2,1) (2,2).
  unsigned int count=0;
  Polygon_scan it(poly, false);
  int y=0;
  for (it.reset(); it.next(); ++y) {
    vcl_cout << "Scan line at y=" << it.fscany() << " (" << it.scany()
             << ") goes from " << it.fstartx() << " (" << it.startx()
             << ") to " << it.fendx() << " (" << it.endx() << ")\n";
    TEST("iteration without boundary: y value", it.scany(), y);
    TEST("iteration without boundary: first x value", it.startx(), y);
    TEST("iteration without boundary: last x value", it.endx(), 2);
    count += it.endx() - it.startx() + 1;
  }
  TEST("iteration without boundary: count", count, 6);
}

static void
test_with_boundary()
{
  Polygon_type poly;
  poly.new_sheet();
  // This is a 2x2 square with corner points at non-integer coordinates.
  poly.push_back( Point_type(10.5f, 20.5f) );
  poly.push_back( Point_type(12.5f, 20.5f) );
  poly.push_back( Point_type(12.5f, 22.5f) );
  poly.push_back( Point_type(10.5f, 22.5f) );

  // The "boundary" pixels (which only partially overlap the square) should be
  // included, hence there are 3x4=12 pixels "inside" of this square:
  unsigned int count=0;
  Polygon_scan it(poly);
  int y=21;
  for (it.reset(); it.next(); ++y) {
    vcl_cout << "Scan line at y=" << it.fscany() << " (" << it.scany()
             << ") goes from " << it.fstartx() << " (" << it.startx()
             << ") to " << it.fendx() << " (" << it.endx() << ")\n";
    TEST("iteration with boundary: y value", it.scany(), y);
    TEST("iteration with boundary: first x value", it.startx(), 10);
    TEST("iteration with boundary: last x value", it.endx(), 13);
    count += it.endx() - it.startx() + 1;
  }
  // There are points within the polygon. This includes points with
  // integer valued coordinates and the ones at the boundary
  // (each scan line begins at the boundary of the polygon).
  TEST("iteration with boundary: count", count, 12);
}

static void
test_degenerate_polygon()
{
  Polygon_type poly;
  poly.new_sheet();
  // This is a triangle with 2 sides of length 29 parallel to the axes,
  // at X=39 and Y=10 resp., and with one duplicate corner point.
  poly.push_back( Point_type( 10, 10) );
  poly.push_back( Point_type( 39, 10) );
  poly.push_back( Point_type( 39, 39) );
  poly.push_back( Point_type( 10, 10) );

  // There will be scan lines at y=10 to 39, with x values going from y to 39.
  // Hence the number of integer-valued points is 30+29+...+2+1=30*31/2=465.
  Polygon_scan it( poly );
  unsigned int count=0;
  int y=10;
  for (it.reset(); it.next(); ++y) {
    vcl_cout << "Scan line at y=" << it.fscany() << " (" << it.scany()
             << ") goes from " << it.fstartx() << " (" << it.startx()
             << ") to " << it.fendx() << " (" << it.endx() << ")\n";
    TEST("zero-length edges: y value", it.scany(), y);
    TEST("zero-length edges: first x value", it.startx(), y);
    TEST("zero-length edges: last x value", it.endx(), 39);
    count += it.endx() - it.startx() + 1;
  }
  TEST("zero-length edges", count, 465);
}

static void
test_empty_polygon()
{
  // This is an empty polygon.
  Polygon_type poly;

  Polygon_scan it( poly );
  unsigned int count=0;
  for (it.reset(); it.next(); ) {
    vcl_cout << "Scan line at y=" << it.fscany() << " (" << it.scany()
             << ") goes from " << it.fstartx() << " (" << it.startx()
             << ") to " << it.fendx() << " (" << it.endx() << ")\n";
    count += it.endx() - it.startx() + 1;
  }
  TEST("empty polygon", count, 0);
}

static void
test_almost_horizontal()
{
  Polygon_type poly;
  poly.new_sheet();
  poly.push_back( Point_type(424.f, 785.99975f) );
  poly.push_back( Point_type(426.f, 786.00012f) );
  poly.push_back( Point_type(426.f, 789.91188f) );
  poly.push_back( Point_type(424.f, 790.06225f) );

  // There should be scan lines at y=786,787,788,789 and 790.
  // The 3 middle ones are exactly of length 2, containing 3 integer points.
  // The one at y=786 goes from x=424 to x=,
  // while the one at y=490 goes from x=424 to x=.
  Polygon_scan it(poly,false);
  int y=786;
  for (it.reset(); it.next(); ++y) {
    vcl_cout << "Scan line at y=" << it.fscany() << " (" << it.scany()
             << ") goes from " << it.fstartx() << " (" << it.startx()
             << ") to " << it.fendx() << " (" << it.endx() << ")\n";
    TEST("almost horizontal sides: y value", it.scany(), y);
    TEST("almost horizontal sides: first x value", it.startx(), 424);
    int x = 426; if (y==790) x-=2; if (y==786) --x;
    TEST("almost horizontal sides: last x value", it.endx(), x);
  }
}


static void test_polygon_scan_iterator()
{
  test_without_boundary();
  test_with_boundary();
  test_degenerate_polygon();
  test_empty_polygon();
  test_almost_horizontal();
}

TESTMAIN(test_polygon_scan_iterator);
