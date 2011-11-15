#include <testlib/testlib_register.h>

DECLARE( test_image_pyramid );
DECLARE( test_process_mains ); 
DECLARE( test_weighted_em   ); 

void register_tests()
{
  REGISTER( test_image_pyramid );
  REGISTER( test_process_mains ); 
  REGISTER( test_weighted_em   );
}

DEFINE_MAIN;
