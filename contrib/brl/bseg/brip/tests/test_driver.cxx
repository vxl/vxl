#include <testlib/testlib_register.h>


DECLARE( test_histogram );
DECLARE( test_mutual_info );

void
register_tests()
{
  REGISTER( test_histogram );
  REGISTER( test_mutual_info );
}

DEFINE_MAIN;


