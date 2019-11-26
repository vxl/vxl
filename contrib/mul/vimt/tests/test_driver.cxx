#include "testlib/testlib_register.h"

DECLARE( test_image_2d_of );
DECLARE( test_sample_profile_bilin );
DECLARE( test_sample_grid_bilin );
DECLARE( test_transform_2d );
DECLARE( test_gaussian_pyramid_builder_2d );
DECLARE( test_dog_pyramid_builder_2d );
DECLARE( test_scale_pyramid_builder_2d );
DECLARE( test_gaussian_pyramid_builder_2d_general );
DECLARE( test_find_peaks );
DECLARE( test_find_troughs );
DECLARE( test_correlate_2d );
DECLARE( test_resample_bilin );
DECLARE( test_image_bounds_and_centre_2d );
DECLARE( test_v2i );
DECLARE( test_reflect );

void
register_tests()
{
  REGISTER( test_image_2d_of );
  REGISTER( test_sample_profile_bilin );
  REGISTER( test_sample_grid_bilin );
  REGISTER( test_transform_2d );
  REGISTER( test_gaussian_pyramid_builder_2d );
  REGISTER( test_dog_pyramid_builder_2d );
  REGISTER( test_scale_pyramid_builder_2d );
  REGISTER( test_gaussian_pyramid_builder_2d_general );
  REGISTER( test_find_peaks );
  REGISTER( test_find_troughs );
  REGISTER( test_correlate_2d );
  REGISTER( test_resample_bilin );
  REGISTER( test_image_bounds_and_centre_2d );
  REGISTER( test_v2i );
  REGISTER( test_reflect );
}

DEFINE_MAIN;
