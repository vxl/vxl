#include <testlib/testlib_register.h>

DECLARE(test_gevd_bufferxy);
DECLARE(test_gevd_float_operators);
DECLARE(test_gevd_memory_mixin);
DECLARE(test_gevd_noise);
DECLARE(test_gevd_param_mixin);

void
register_tests()
{
  REGISTER(test_gevd_bufferxy);
  REGISTER(test_gevd_float_operators);
  REGISTER(test_gevd_memory_mixin);
  REGISTER(test_gevd_noise);
  REGISTER(test_gevd_param_mixin);
}

DEFINE_MAIN;
