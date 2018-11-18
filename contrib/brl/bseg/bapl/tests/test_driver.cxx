#include <testlib/testlib_register.h>


DECLARE( test_transform );
DECLARE( test_roi );
DECLARE( test_matcher );
DECLARE( test_lowe_pyramid_set );
DECLARE( test_compute_tracks );
DECLARE( test_match_keypoints );
DECLARE( test_dense_sift );

void
register_tests()
{
  REGISTER( test_transform );
  REGISTER( test_roi );
  REGISTER( test_matcher );
  REGISTER( test_lowe_pyramid_set );
  REGISTER( test_compute_tracks );
  REGISTER( test_match_keypoints );
  REGISTER( test_dense_sift );
}

DEFINE_MAIN;
