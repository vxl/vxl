#include <testlib/testlib_register.h>

DECLARE( test_bundler_utils );
DECLARE( test_tracks_detect );
DECLARE( test_propose_matches );
DECLARE( test_match_ann );

void register_tests()
{
  REGISTER( test_bundler_utils );
  REGISTER( test_tracks_detect );
  REGISTER( test_propose_matches );
  REGISTER( test_match_ann );
}

DEFINE_MAIN;
