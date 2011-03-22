#include <testlib/testlib_register.h>

DECLARE(test_detector);
DECLARE(test_region_proc);
DECLARE(test_gauss_fit);
DECLARE(test_image_mesh);

void
register_tests()
{
  REGISTER(test_detector);
  REGISTER(test_region_proc);
  REGISTER(test_gauss_fit);
  REGISTER(test_image_mesh);
}

DEFINE_MAIN;
