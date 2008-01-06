#include <testlib/testlib_register.h>


DECLARE(test_cam_iostream);
DECLARE(test_video_site_io);
DECLARE(test_video_corr_processor);

  
void
register_tests()
{
  REGISTER(test_cam_iostream);
  REGISTER(test_video_site_io);
  REGISTER(test_video_corr_processor);
}

DEFINE_MAIN;
