#include <testlib/testlib_register.h>

DECLARE( test_proj_camera );
DECLARE( test_perspective_camera );
DECLARE( test_affine_camera );
DECLARE( test_fundamental_matrix );
DECLARE( test_calibration_matrix );
DECLARE( test_camera_io );
DECLARE( test_poly_radial_distortion );

void register_tests()
{
  REGISTER( test_proj_camera );
  REGISTER( test_perspective_camera );
  REGISTER( test_affine_camera );
  REGISTER( test_fundamental_matrix );
  REGISTER( test_calibration_matrix );
  REGISTER( test_camera_io );
  REGISTER( test_poly_radial_distortion );
}

DEFINE_MAIN;
