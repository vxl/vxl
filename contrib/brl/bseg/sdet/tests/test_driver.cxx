#include <testlib/testlib_register.h>

DECLARE(test_detector);
DECLARE(test_region_proc);
DECLARE(test_gauss_fit);
DECLARE(test_image_mesh);
DECLARE(test_denoise_mrf);

void
register_tests()
{
  REGISTER(test_detector);
  REGISTER(test_region_proc);
  REGISTER(test_gauss_fit);
  REGISTER(test_image_mesh);
  REGISTER(test_denoise_mrf);
}

DEFINE_MAIN;
