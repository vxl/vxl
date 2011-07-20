#include <testlib/testlib_register.h>

DECLARE( test_image_pyramid );
DECLARE( test_process_mains ); 

void register_tests()
{
  REGISTER( test_image_pyramid );
  REGISTER( test_process_mains ); 
}

DEFINE_MAIN;
