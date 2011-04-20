#include <testlib/testlib_register.h>

DECLARE( test_merge_mixtures );
DECLARE( test_cone_ray_trace ); 

void register_tests()
{
  REGISTER( test_merge_mixtures );
  REGISTER( test_cone_ray_trace ); 
}


DEFINE_MAIN;
