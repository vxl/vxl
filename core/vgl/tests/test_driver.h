#ifndef VGL_TEST_DRIVER_H_
#define VGL_TEST_DRIVER_H_

bool close( double x, double y );

vgl_polygon
new_polygon_impl( float points[], unsigned int n );

void
add_contour_impl( vgl_polygon& poly, float points[], unsigned int n );

#define new_polygon( P ) new_polygon_impl( P, sizeof(P) / sizeof(float) )
#define add_contour( POLY, P ) add_contour_impl( POLY, P, sizeof(P) / sizeof(float) )

#endif
