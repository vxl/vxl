#include <testlib/testlib_register.h>


DECLARE( test_cvmatch );
DECLARE( test_clsd_cvmatch );

void
register_tests()
{
  REGISTER( test_cvmatch );
  REGISTER( test_clsd_cvmatch );
}

DEFINE_MAIN;
