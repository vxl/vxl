#include <testlib/testlib_register.h>

DECLARE(test_vdgl);
DECLARE(test1_vdgl);
DECLARE(test_vdgl_io);

void
register_tests()
{
  REGISTER(test_vdgl);
  REGISTER(test1_vdgl);
  REGISTER(test_vdgl_io);
}

DEFINE_MAIN;
