#include <testlib/testlib_register.h>

DECLARE( test_fill_in_mesh );
DECLARE( test_save_scene_raw );
DECLARE( test_upload_mesh );
DECLARE( test_boxm_fill_internal_cells );
DECLARE( test_boxm_scene_crop );
DECLARE( test_linear_operations );
DECLARE( test_remove_level0 );

void register_tests()
{
  REGISTER( test_fill_in_mesh );
  REGISTER( test_save_scene_raw );
  REGISTER( test_upload_mesh );
  REGISTER( test_boxm_fill_internal_cells );
  REGISTER( test_boxm_scene_crop );
  REGISTER( test_linear_operations );
  REGISTER( test_remove_level0 );
}


DEFINE_MAIN;
