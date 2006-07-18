#include <testlib/testlib_register.h>

DECLARE( test_make_ortho_projection );
DECLARE( test_rotation_from_ortho_projection );
DECLARE( test_ortho_rigid_builder );
DECLARE( test_ortho_flexible_builder );
DECLARE( test_rotation_matrix );
DECLARE( test_correction_matrix_error );

void
register_tests()
{
  REGISTER( test_make_ortho_projection );
  REGISTER( test_rotation_from_ortho_projection );
  REGISTER( test_ortho_rigid_builder );
  REGISTER( test_ortho_flexible_builder );
  REGISTER( test_rotation_matrix );
  REGISTER( test_correction_matrix_error );
}

DEFINE_MAIN;
