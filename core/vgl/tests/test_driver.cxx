#include <testlib/testlib_register.h>

DECLARE( test_cartesian );
DECLARE( test_distance );
DECLARE( test_polygon );
DECLARE( test_convex );
DECLARE( test_polygon_scan_iterator );
DECLARE( test_triangle_scan_iterator );
DECLARE( test_ellipse_scan_iterator );
DECLARE( test_window_scan_iterator );
DECLARE( test_area );
DECLARE( test_clip );
DECLARE( test_closest_point );
DECLARE( test_sphere );
DECLARE( test_line_3d_2_points );
DECLARE( test_line_segment_3d );
DECLARE( test_triangle_3d );
DECLARE( test_triangle_3d_line_intersection );
DECLARE( test_infinite_line_3d );
DECLARE( test_ray_3d );
DECLARE( test_plane_3d );
DECLARE( test_frustum_3d );
DECLARE( test_intersection );
DECLARE( test_spline );
DECLARE( test_pointset );
DECLARE( test_quadric );
DECLARE( test_oriented_box_2d );
DECLARE( test_affine_coordinates );

void
register_tests()
{
  REGISTER( test_cartesian );
  REGISTER( test_distance );
  REGISTER( test_polygon );
  REGISTER( test_convex );
  REGISTER( test_polygon_scan_iterator );
  REGISTER( test_triangle_scan_iterator );
  REGISTER( test_ellipse_scan_iterator );
  REGISTER( test_window_scan_iterator );
  REGISTER( test_area );
  REGISTER( test_clip );
  REGISTER( test_closest_point );
  REGISTER( test_sphere );
  REGISTER( test_line_3d_2_points );
  REGISTER( test_line_segment_3d );
  REGISTER( test_triangle_3d );
  REGISTER( test_triangle_3d_line_intersection );
  REGISTER( test_infinite_line_3d );
  REGISTER( test_ray_3d );
  REGISTER( test_plane_3d );
  REGISTER( test_frustum_3d );
  REGISTER( test_intersection );
  REGISTER( test_spline );
  REGISTER( test_pointset );
  REGISTER( test_quadric );
  REGISTER( test_oriented_box_2d );
  REGISTER( test_affine_coordinates );
}

DEFINE_MAIN;
