#include <testlib/testlib_register.h>

DECLARE( test_compute_similarity_3d );
DECLARE( test_compute_rigid_3d );
DECLARE( test_conic );
DECLARE( test_convex_hull_2d );
DECLARE( test_fit_conics_2d );
DECLARE( test_fit_lines_2d );
DECLARE( test_fit_plane_3d );
DECLARE( test_fit_sphere_3d );
DECLARE( test_fit_quadric_3d );
DECLARE( test_h_matrix_1d );
DECLARE( test_h_matrix_2d );
DECLARE( test_h_matrix_3d );
DECLARE( test_homg );
DECLARE( test_intersection );
DECLARE( test_orient_box_3d );
DECLARE( test_p_matrix );
DECLARE( test_rotation_3d );
DECLARE( test_rtree );

void
register_tests()
{
  REGISTER( test_compute_similarity_3d );
  REGISTER( test_compute_rigid_3d );
  REGISTER( test_conic );
  REGISTER( test_convex_hull_2d );
  REGISTER( test_fit_conics_2d );
  REGISTER( test_fit_lines_2d );
  REGISTER( test_fit_plane_3d );
  REGISTER( test_fit_sphere_3d );
  REGISTER( test_fit_quadric_3d );
  REGISTER( test_h_matrix_1d );
  REGISTER( test_h_matrix_2d );
  REGISTER( test_h_matrix_3d );
  REGISTER( test_homg );
  REGISTER( test_intersection );
  REGISTER( test_orient_box_3d );
  REGISTER( test_p_matrix );
  REGISTER( test_rotation_3d );
  REGISTER( test_rtree );
}

DEFINE_MAIN;
