#include <testlib/testlib_register.h>

DECLARE( test_binary_io );
DECLARE( test_block_iter );
DECLARE( test_block_vis_graph );
DECLARE( test_cell_vis_graph );
DECLARE( test_quad_interpolate );
DECLARE( test_save_raw );
DECLARE( test_render_image );
DECLARE( test_update );
DECLARE( test_compute_visibility );
DECLARE( test_update_multi_bin );
DECLARE( test_boxm_rational_camera_utils);
DECLARE( test_upload_mesh );
DECLARE( test_fill_in_mesh );
DECLARE( test_save_scene_raw );

void register_tests()
{
  REGISTER( test_boxm_rational_camera_utils);
  REGISTER( test_binary_io );
  REGISTER( test_block_iter );
  REGISTER( test_block_vis_graph );
  REGISTER( test_cell_vis_graph );
  REGISTER( test_quad_interpolate );
  REGISTER( test_save_raw );
  REGISTER( test_render_image );
  REGISTER( test_update );
  REGISTER( test_compute_visibility );
  REGISTER( test_update_multi_bin );
  REGISTER( test_upload_mesh );
  REGISTER( test_fill_in_mesh );
  REGISTER( test_save_scene_raw );
}


DEFINE_MAIN;
