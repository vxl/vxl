#include <testlib/testlib_register.h>

DECLARE( test_lvcs_io );
DECLARE( test_camera_io );
DECLARE( test_proj_camera_io );
DECLARE( test_perspective_camera_io );
DECLARE( test_affine_camera_io );
DECLARE( test_rational_camera_io );
DECLARE( test_local_rational_camera_io );

void register_tests()
{
  REGISTER( test_lvcs_io );
  REGISTER( test_camera_io );
  REGISTER( test_proj_camera_io );
  REGISTER( test_perspective_camera_io );
  REGISTER( test_affine_camera_io );
  REGISTER( test_rational_camera_io );
  REGISTER( test_local_rational_camera_io );
}

DEFINE_MAIN;
