#include <testlib/testlib_register.h>

DECLARE( test_rgrl_util );
DECLARE( test_transformation );
DECLARE( test_trans_reader );
DECLARE( test_feature );
DECLARE( test_feature_set );
DECLARE( test_match_set );
DECLARE( test_spline );
DECLARE( test_estimator );
DECLARE( test_convergence );
DECLARE( test_scale_est );
DECLARE( test_initializer );
DECLARE( test_set_of );
DECLARE( test_initializer_ran_sam );
DECLARE( test_matcher );
DECLARE( test_image_convert );

void
register_tests()
{
  REGISTER( test_rgrl_util );
  REGISTER( test_transformation );
  REGISTER( test_feature );
  REGISTER( test_trans_reader );
  REGISTER( test_feature_set );
  REGISTER( test_match_set );
  REGISTER( test_spline );
  REGISTER( test_estimator );
  REGISTER( test_convergence );
  REGISTER( test_scale_est );
  REGISTER( test_initializer );
  REGISTER( test_set_of );
  REGISTER( test_initializer_ran_sam );
  REGISTER( test_matcher );
  REGISTER( test_image_convert );
}

DEFINE_MAIN;
