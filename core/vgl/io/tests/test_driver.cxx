#include <testlib/testlib_register.h>

DECLARE( golden_test_vgl_io );
DECLARE( test_box_2d_io );
DECLARE( test_box_3d_io );
DECLARE( test_conic_io );
DECLARE( test_homg_line_2d_io );
DECLARE( test_homg_line_3d_2_points_io );
DECLARE( test_homg_plane_3d_io );
DECLARE( test_homg_point_1d_io );
DECLARE( test_homg_point_2d_io );
DECLARE( test_homg_point_3d_io );
DECLARE( test_line_2d_io );
DECLARE( test_line_3d_2_points_io );
DECLARE( test_line_segment_2d_io );
DECLARE( test_line_segment_3d_io );
DECLARE( test_point_2d_io );
DECLARE( test_point_3d_io );
DECLARE( test_plane_3d_io );
DECLARE( test_polygon_io );
DECLARE( test_vector_2d_io );
DECLARE( test_vector_3d_io );

void
register_tests()
{
  REGISTER( golden_test_vgl_io );
  REGISTER( test_box_2d_io );
  REGISTER( test_box_3d_io );
  REGISTER( test_conic_io );
  REGISTER( test_homg_line_2d_io );
  REGISTER( test_homg_line_3d_2_points_io );
  REGISTER( test_homg_plane_3d_io );
  REGISTER( test_homg_point_1d_io );
  REGISTER( test_homg_point_2d_io );
  REGISTER( test_homg_point_3d_io );
  REGISTER( test_line_2d_io );
  REGISTER( test_line_3d_2_points_io );
  REGISTER( test_line_segment_2d_io );
  REGISTER( test_line_segment_3d_io );
  REGISTER( test_point_2d_io );
  REGISTER( test_point_3d_io );
  REGISTER( test_plane_3d_io );
  REGISTER( test_polygon_io );
  REGISTER( test_vector_2d_io );
  REGISTER( test_vector_3d_io );
}

DEFINE_MAIN;
