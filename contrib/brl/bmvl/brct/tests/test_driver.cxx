#include <testlib/testlib_register.h>

DECLARE(brct_test_synthetic_data);
DECLARE(brct_test_compute_P);

void
register_tests()
{
  REGISTER(brct_test_synthetic_data);
  REGISTER(brct_test_compute_P);
}

DEFINE_MAIN;
