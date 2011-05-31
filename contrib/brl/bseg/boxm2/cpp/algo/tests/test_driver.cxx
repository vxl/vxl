#include <testlib/testlib_register.h>

DECLARE( test_merge_mixtures );
DECLARE( test_cone_ray_trace ); 
DECLARE( test_cone_update ); 

void register_tests()
{
  REGISTER( test_merge_mixtures );
  REGISTER( test_cone_ray_trace ); 
  REGISTER( test_cone_update ); 
}


DEFINE_MAIN;
