#include <testlib/testlib_register.h>

DECLARE( test_transform_3d );
DECLARE( test_sample_grid_trilin );
DECLARE( test_image_3d_of );
DECLARE( test_load );
DECLARE( test_v3i );
DECLARE( test_from_image_2d );
DECLARE( test_resample );
DECLARE( test_gauss_reduce );

void
register_tests()
{
  REGISTER( test_transform_3d );
  REGISTER( test_image_3d_of );
  REGISTER( test_load );
  REGISTER( test_sample_grid_trilin );
  REGISTER( test_v3i );
  REGISTER( test_from_image_2d );
  REGISTER( test_resample );
  REGISTER( test_gauss_reduce );
}

DEFINE_MAIN;
