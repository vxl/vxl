#include <testlib/testlib_register.h>


DECLARE( test_io );
DECLARE( test_update_dist_image );
DECLARE( test_neighborhood_ops );

void
register_tests()
{
  REGISTER( test_io );
  REGISTER( test_update_dist_image );
  REGISTER( test_neighborhood_ops );
}

DEFINE_MAIN;



