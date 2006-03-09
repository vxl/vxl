#include <testlib/testlib_register.h>


//DECLARE( test_construct_cameras);
DECLARE( test_project );
DECLARE( test_optimize_camera );
DECLARE( test_fm_compute );
DECLARE( test_camera_compute );
DECLARE( test_lens_warp_mapper );

void register_tests()
{
  //  REGISTER( test_construct_cameras );
  REGISTER( test_project );
  REGISTER( test_optimize_camera );
  REGISTER( test_fm_compute );
  REGISTER( test_camera_compute );
  REGISTER( test_lens_warp_mapper );
}

DEFINE_MAIN;
