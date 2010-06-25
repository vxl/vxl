#include <testlib/testlib_register.h>

DECLARE( test_merge_mog );
DECLARE( test_mog_norm );
DECLARE( test_boxm_scene_to_bvxm_grid);


void register_tests()
{
  REGISTER( test_merge_mog );
  REGISTER( test_mog_norm );
  REGISTER( test_boxm_scene_to_bvxm_grid );

}

DEFINE_MAIN;
