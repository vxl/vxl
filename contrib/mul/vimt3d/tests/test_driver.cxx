#include <testlib/testlib_register.h>

DECLARE( test_transform_3d );
DECLARE( test_sample_grid_trilin );
DECLARE( test_image_3d_of );
DECLARE( test_load );
DECLARE( test_save );
DECLARE( test_v3i );
DECLARE( test_v3m );
DECLARE( test_from_image_2d );
DECLARE( test_resample );
DECLARE( test_gauss_reduce );
DECLARE( test_image_bounds_and_centre_3d );
DECLARE( test_reflect );
DECLARE( test_crop );

void
register_tests()
{
  REGISTER( test_transform_3d );
  REGISTER( test_image_3d_of );
  REGISTER( test_load );
  REGISTER( test_save );
  REGISTER( test_sample_grid_trilin );
  REGISTER( test_v3i );
  REGISTER( test_v3m );
  REGISTER( test_from_image_2d );
  REGISTER( test_resample );
  REGISTER( test_gauss_reduce );
  REGISTER( test_image_bounds_and_centre_3d );
  REGISTER( test_reflect );
  REGISTER( test_crop );
}

DEFINE_MAIN;
