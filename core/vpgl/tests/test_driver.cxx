#include "testlib/testlib_register.h"

DECLARE(test_proj_camera);
DECLARE(test_perspective_camera);
DECLARE(test_perspective_camera_decomposition);
DECLARE(test_affine_camera);
DECLARE(test_fundamental_matrix);
DECLARE(test_essential_matrix);
DECLARE(test_calibration_matrix);
DECLARE(test_poly_radial_distortion);
DECLARE(test_rad_tan_distortion);
DECLARE(test_rational_camera);
DECLARE(test_local_rational_camera);
DECLARE(test_generic_camera);
DECLARE(test_lvcs);
DECLARE(test_utm);
DECLARE(test_tri_focal_tensor);
DECLARE(test_affine_tri_focal_tensor);
DECLARE(test_affine_fundamental_matrix);
DECLARE(test_RSM_camera);

void
register_tests()
{
  REGISTER(test_proj_camera);
  REGISTER(test_perspective_camera);
  REGISTER(test_perspective_camera_decomposition);
  REGISTER(test_affine_camera);
  REGISTER(test_fundamental_matrix);
  REGISTER(test_essential_matrix);
  REGISTER(test_calibration_matrix);
  REGISTER(test_poly_radial_distortion);
  REGISTER(test_rad_tan_distortion);
  REGISTER(test_rational_camera);
  REGISTER(test_local_rational_camera);
  REGISTER(test_generic_camera);
  REGISTER(test_lvcs);
  REGISTER(test_utm);
  REGISTER(test_tri_focal_tensor);
  REGISTER(test_affine_tri_focal_tensor);
  REGISTER(test_affine_fundamental_matrix);
  REGISTER(test_RSM_camera);
}

DEFINE_MAIN;
