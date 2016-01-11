#include <testlib/testlib_register.h>

DECLARE( test_compute_visibility );
DECLARE( test_render_image );
DECLARE( test_update );
DECLARE( test_update_multi_bin );

void register_tests()
{
  REGISTER( test_compute_visibility );
  REGISTER( test_render_image );
  REGISTER( test_update );
  REGISTER( test_update_multi_bin );
}


DEFINE_MAIN;
