#include <testlib/testlib_register.h>


DECLARE( test_io );
DECLARE( test_find );

void
register_tests()
{
  REGISTER( test_io );
  REGISTER( test_find );
}

DEFINE_MAIN;



