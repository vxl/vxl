#include <testlib/testlib_register.h>

DECLARE( test_boxm_plane_ransac );
DECLARE( test_boxm_rational_camera_utils );
DECLARE( test_quad_interpolate );

void register_tests()
{
  REGISTER( test_boxm_plane_ransac );
  REGISTER( test_boxm_rational_camera_utils );
  REGISTER( test_quad_interpolate );
}


DEFINE_MAIN;
