#include <testlib/testlib_register.h>

DECLARE(sdet_test_detector);
DECLARE(sdet_test_region_proc);
DECLARE(brip_test_lucas_kanade);
DECLARE(brip_test_fourier);

void
register_tests()
{
  REGISTER(sdet_test_detector);
  REGISTER(sdet_test_region_proc);
  REGISTER(brip_test_lucas_kanade);
  REGISTER(brip_test_fourier);
}

DEFINE_MAIN;
