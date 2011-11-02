#include <testlib/testlib_register.h>


DECLARE( test_minimizer );
DECLARE( test_matcher_translation );

void
register_tests()
{
  REGISTER( test_minimizer );
  REGISTER( test_matcher_translation );
}

DEFINE_MAIN;


