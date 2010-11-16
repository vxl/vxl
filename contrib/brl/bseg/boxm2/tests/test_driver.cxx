#include <testlib/testlib_register.h>

DECLARE( test_scene );
DECLARE( test_cache );
DECLARE( test_io );
DECLARE( test_wrappers );

void register_tests()
{
  REGISTER( test_scene );
  REGISTER( test_cache );
  REGISTER( test_io );
  REGISTER( test_wrappers );
}


DEFINE_MAIN;
