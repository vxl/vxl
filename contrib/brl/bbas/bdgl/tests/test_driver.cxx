#include <testlib/testlib_register.h>

DECLARE(test_curve_algs);
DECLARE(test_region_algs);

void
register_tests()
{
  REGISTER(test_curve_algs);
  REGISTER(test_region_algs);
}

DEFINE_MAIN;
