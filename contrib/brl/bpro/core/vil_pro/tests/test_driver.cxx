#include <testlib/testlib_register.h>

DECLARE( test_vil_convert_to_n_planes_process );
DECLARE( test_vil_crop_image_process );


void register_tests()
{
  REGISTER( test_vil_convert_to_n_planes_process );
  REGISTER( test_vil_crop_image_process );
}

DEFINE_MAIN;
