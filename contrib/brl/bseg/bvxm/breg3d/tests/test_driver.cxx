#include <testlib/testlib_register.h>

DECLARE( test_ekf_camera_optimizer );
DECLARE( test_homography_generator );
DECLARE( test_ekf_camera_optimizer_seq );



void register_tests()
{
  //REGISTER( test_homography_generator );
  //REGISTER( test_ekf_camera_optimizer );
  REGISTER( test_ekf_camera_optimizer_seq );
  

}

DEFINE_MAIN;
