#include <testlib/testlib_register.h>
// TESTS FAIL ON DASHBOARD FIX_ME
DECLARE( test_ekf_camera_optimizer );
//DECLARE( test_homography_generator );
//DECLARE( test_ekf_camera_optimizer_seq );
//DECLARE( test_ekf_existing_world );



void register_tests()
{
// TESTS FAIL ON DASHBOARD FIX_ME
  //  REGISTER( test_homography_generator );
   REGISTER( test_ekf_camera_optimizer );
  //REGISTER( test_ekf_camera_optimizer_seq );
  // REGISTER( test_ekf_existing_world )
}

DEFINE_MAIN;
