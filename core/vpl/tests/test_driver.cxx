// This is core/vpl/tests/test_driver.cxx
#include <testlib/testlib_register.h>

DECLARE( test_unistd );

void
register_tests()
{
  REGISTER( test_unistd );
}

DEFINE_MAIN;
