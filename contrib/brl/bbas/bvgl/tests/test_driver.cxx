#include <testlib/testlib_register.h>

DECLARE(test_articulated_poly);
DECLARE(test_misc);


void
register_tests()
{
  REGISTER(test_articulated_poly);
  REGISTER(test_misc);
}

DEFINE_MAIN;
