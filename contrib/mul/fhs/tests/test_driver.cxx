#include <testlib/testlib_register.h>

DECLARE( test_arc );
DECLARE( test_searcher );

void
register_tests()
{
  REGISTER( test_arc );
  REGISTER( test_searcher );
}

DEFINE_MAIN;
