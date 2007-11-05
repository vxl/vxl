#include <testlib/testlib_register.h>

DECLARE(test_detector);
DECLARE(test_region_proc);
DECLARE(test_gauss_fit);

void
register_tests()
{
  REGISTER(test_detector);
  REGISTER(test_region_proc);
  REGISTER(test_gauss_fit);
}

DEFINE_MAIN;
