#include <testlib/testlib_register.h>

DECLARE( test_convert );
DECLARE( test_file_format_read );
DECLARE( test_pixel_format );
DECLARE( test_save_load_image );
DECLARE( test_image_loader_robustness );
DECLARE( test_stream );
DECLARE( test_image_view );
DECLARE( test_image_resource );
DECLARE( test_bilin_interp );
DECLARE( test_sample_profile_bilin );
DECLARE( test_sample_grid_bilin );
DECLARE( test_resample_bilin );
DECLARE( test_bicub_interp );
DECLARE( test_sample_profile_bicub );
DECLARE( test_sample_grid_bicub );
DECLARE( test_resample_bicub );
DECLARE( test_image_view_maths );
DECLARE( test_memory_chunk );
DECLARE( test_deep_copy_3_plane );
DECLARE( test_rotate_image );
DECLARE( test_warp );

void
register_tests()
{
  REGISTER( test_image_view_maths );
  REGISTER( test_convert );
  REGISTER( test_pixel_format );
  REGISTER( test_save_load_image );
  REGISTER( test_file_format_read );
  REGISTER( test_image_loader_robustness );
  REGISTER( test_stream );
  REGISTER( test_image_view );
  REGISTER( test_image_resource );
  REGISTER( test_bilin_interp );
  REGISTER( test_sample_profile_bilin );
  REGISTER( test_sample_grid_bilin );
  REGISTER( test_resample_bilin );
  REGISTER( test_bicub_interp );
  REGISTER( test_sample_profile_bicub );
  REGISTER( test_sample_grid_bicub );
  REGISTER( test_resample_bicub );
  REGISTER( test_memory_chunk );
  REGISTER( test_deep_copy_3_plane );
  REGISTER( test_rotate_image );
  REGISTER( test_warp );
}

DEFINE_MAIN;
