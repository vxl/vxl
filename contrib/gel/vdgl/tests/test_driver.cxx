#include <testlib/testlib_register.h>

DECLARE(test_vdgl);
DECLARE(test1_vdgl);

void
register_tests()
{
  REGISTER(test_vdgl);
  REGISTER(test1_vdgl);
}

DEFINE_MAIN;
