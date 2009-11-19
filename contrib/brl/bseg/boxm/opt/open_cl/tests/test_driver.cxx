#include <testlib/testlib_register.h>

DECLARE( test_octree );
DECLARE( test_ray_trace );

void register_tests()
{
  REGISTER( test_octree );
  REGISTER( test_ray_trace );
}


DEFINE_MAIN;
