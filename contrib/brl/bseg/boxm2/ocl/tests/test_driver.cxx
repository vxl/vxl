#include <testlib/testlib_register.h>

DECLARE( test_image_pyramid );
DECLARE( test_process_mains ); 
DECLARE( test_weighted_em   ); 
DECLARE( test_kernel_filter );
DECLARE( test_kernel_vector_filter );

void register_tests()
{
  REGISTER( test_image_pyramid );
  REGISTER( test_process_mains ); 
  REGISTER( test_weighted_em   );
  REGISTER( test_kernel_filter );
  REGISTER( test_kernel_vector_filter );

}

DEFINE_MAIN;
