#include <testlib/testlib_register.h>

DECLARE( test_boxm_plane_ransac );
DECLARE( test_boxm_rational_camera_utils );
DECLARE( test_quad_interpolate );
DECLARE( test_sigma_normalizer );

void register_tests()
{
  REGISTER( test_boxm_plane_ransac );
  REGISTER( test_boxm_rational_camera_utils );
  REGISTER( test_quad_interpolate );
  REGISTER( test_sigma_normalizer );
}


DEFINE_MAIN;
