#include <testlib/testlib_register.h>


DECLARE( test_optimize_camera );
DECLARE( test_fm_compute );
DECLARE( test_construct_cameras );
DECLARE( test_interpolate );
DECLARE( test_camera_homographies);
DECLARE( test_camera_from_box );
DECLARE( test_project );
DECLARE( test_gridding );

void register_tests()
{
  REGISTER( test_optimize_camera );
  REGISTER( test_fm_compute );
  REGISTER( test_construct_cameras );
  REGISTER( test_interpolate );
  REGISTER( test_camera_homographies );
  REGISTER( test_camera_from_box );
  REGISTER( test_project );
  REGISTER( test_gridding );
}

DEFINE_MAIN;
