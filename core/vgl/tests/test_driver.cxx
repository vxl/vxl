#include <testlib/testlib_register.h>

DECLARE( test_cartesian );
DECLARE( test_distance );
DECLARE( test_conic );
DECLARE( test_homg );
DECLARE( test_polygon );
DECLARE( test_polygon_scan_iterator );
DECLARE( test_triangle_scan_iterator );
DECLARE( test_ellipse_scan_iterator );
DECLARE( test_area );
DECLARE( test_clip );

void
register_tests()
{
  REGISTER( test_cartesian );
  REGISTER( test_distance );
  REGISTER( test_conic );
  REGISTER( test_homg );
  REGISTER( test_polygon );
  REGISTER( test_polygon_scan_iterator );
  REGISTER( test_triangle_scan_iterator );
  REGISTER( test_ellipse_scan_iterator );
  REGISTER( test_area );
  REGISTER( test_clip );
}


#include <vgl/vgl_polygon.h>
#include <vcl_cmath.h>

vgl_polygon
new_polygon_impl( float points[], unsigned int n )
{
  vgl_polygon poly;

  poly.new_sheet();
  for( unsigned int i=0; i < n; i += 2 ) {
    poly.push_back( points[i], points[i+1] );
  }

  return poly;
}

void
add_contour_impl( vgl_polygon& poly, float points[], unsigned int n )
{
  poly.new_sheet();
  for( unsigned int i=0; i < n; i += 2 ) {
    poly.push_back( points[i], points[i+1] );
  }
}

bool
close( double x, double y )
{
  return vcl_fabs(x-y) < 1.0e-6;
}

DEFINE_MAIN;
