#include <testlib/testlib_register.h>

DECLARE( test_illum );
DECLARE( test_sun_pos );
DECLARE( test_sun_hist );
DECLARE( test_sun_dir_index );
DECLARE( test_feature_pca );
DECLARE( test_phongs_model_est );
DECLARE( test_synoptic_function );
DECLARE( test_image_metadata );
DECLARE( test_atmospheric_parameters );
DECLARE(test_appearance_neighborhood_index );
DECLARE(test_belief_prop_utils );
void register_tests()
{
  REGISTER( test_illum );
  REGISTER( test_sun_pos );
  REGISTER( test_sun_hist );
  REGISTER( test_sun_dir_index );
  REGISTER( test_feature_pca );
  REGISTER( test_phongs_model_est );
  REGISTER( test_synoptic_function );
  REGISTER( test_image_metadata );
  REGISTER( test_atmospheric_parameters );
  REGISTER( test_appearance_neighborhood_index );
  REGISTER( test_belief_prop_utils );
}

DEFINE_MAIN;
