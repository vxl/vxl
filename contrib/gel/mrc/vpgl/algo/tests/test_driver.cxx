#include <testlib/testlib_register.h>


//DECLARE( test_construct_cameras);
DECLARE( test_project );
DECLARE( test_optimize_camera );
DECLARE( test_fm_compute );
DECLARE( test_camera_compute );
DECLARE( test_construct_cameras );
DECLARE( test_lens_warp_mapper );
DECLARE( test_backproject );
DECLARE( test_ray );
DECLARE( test_ortho_procrustes );
DECLARE( test_ray_intersect );
DECLARE( test_adjust_rational_trans_onept );
DECLARE( test_rational_geo_adjust );
DECLARE( test_bundle_adjust );
DECLARE( test_interpolate );
DECLARE( test_camera_homographies);


void register_tests()
{
  //  REGISTER( test_construct_cameras );
  REGISTER( test_project );
  REGISTER( test_optimize_camera );
  REGISTER( test_fm_compute );
  REGISTER( test_camera_compute );
  REGISTER( test_construct_cameras );
  REGISTER( test_lens_warp_mapper );
  REGISTER( test_backproject );
  REGISTER( test_ray );
  REGISTER( test_ortho_procrustes );
  REGISTER( test_ray_intersect );
  REGISTER( test_adjust_rational_trans_onept );
  REGISTER( test_rational_geo_adjust );
  REGISTER( test_bundle_adjust );
  REGISTER( test_interpolate );
  REGISTER( test_camera_homographies );
}

DEFINE_MAIN;
