#include <testlib/testlib_register.h>


DECLARE( test_io );
DECLARE( test_update_dist_image );
#if 0
DECLARE( test_neighborhood_ops );
#endif
void
register_tests()
{
  REGISTER( test_io );
  REGISTER( test_update_dist_image );
#if 0
  REGISTER( test_neighborhood_ops );
#endif
}

DEFINE_MAIN;
