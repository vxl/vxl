#include <testlib/testlib_register.h>

DECLARE(test_detector);
DECLARE(test_region_proc);

void
register_tests()
{
  REGISTER(test_detector);
  REGISTER(test_region_proc);
}

DEFINE_MAIN;
