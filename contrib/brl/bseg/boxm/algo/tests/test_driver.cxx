#include <testlib/testlib_register.h>

DECLARE( test_boxm_scene_crop );
DECLARE( test_boxm_scene_to_bvxm_grid );
DECLARE( test_boxm_fill_internal_cells );
DECLARE( test_boxm_plane_ransac );

void register_tests()
{
  REGISTER( test_boxm_scene_crop );
  REGISTER( test_boxm_scene_to_bvxm_grid );
  REGISTER( test_boxm_fill_internal_cells );
  REGISTER( test_boxm_plane_ransac );
}


DEFINE_MAIN;
