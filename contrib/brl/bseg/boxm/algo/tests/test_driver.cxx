#include <testlib/testlib_register.h>

DECLARE( test_boxm_scene_crop );
DECLARE( test_boxm_scene_to_bvxm_grid );
DECLARE( test_boxm_fill_internal_cells );


void register_tests()
{
  REGISTER( test_boxm_scene_crop );
  REGISTER( test_boxm_scene_to_bvxm_grid );
  REGISTER( test_boxm_fill_internal_cells );
}


DEFINE_MAIN;
