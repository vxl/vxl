#include <testlib/testlib_register.h>

DECLARE( test_cartesian );
DECLARE( test_distance );
DECLARE( test_conic );
DECLARE( test_homg );
DECLARE( test_polygon );
DECLARE( test_polygon_scan_iterator );
DECLARE( test_triangle_scan_iterator );
DECLARE( test_ellipse_scan_iterator );
DECLARE( test_window_scan_iterator );
DECLARE( test_area );
DECLARE( test_clip );
DECLARE( test_h_matrix_2d );
DECLARE( test_h_matrix_3d );
DECLARE( test_fit_lines_2d );
DECLARE( test_p_matrix );
DECLARE( test_closest_point );

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
  REGISTER( test_window_scan_iterator );
  REGISTER( test_area );
  REGISTER( test_clip );
  REGISTER( test_h_matrix_2d );
  REGISTER( test_h_matrix_3d );
  REGISTER( test_fit_lines_2d );
  REGISTER( test_p_matrix );
  REGISTER( test_closest_point );
}

DEFINE_MAIN;
