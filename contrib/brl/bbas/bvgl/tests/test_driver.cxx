#include <testlib/testlib_register.h>

DECLARE(bvgl_test_h_matrix_1d);
DECLARE(bvgl_test_h_matrix_2d);
DECLARE(bvgl_test_h_matrix_3d);
DECLARE(bvgl_test_h_matrix_2d_compute_linear);
DECLARE(bvgl_test_h_matrix_2d_compute_4point);
DECLARE(bvgl_test_norm_trans_2d);

void
register_tests()
{
  REGISTER(bvgl_test_h_matrix_1d);
  REGISTER(bvgl_test_h_matrix_2d);
  REGISTER(bvgl_test_h_matrix_3d);
  REGISTER(bvgl_test_h_matrix_2d_compute_linear);
  REGISTER(bvgl_test_h_matrix_2d_compute_4point);
  REGISTER(bvgl_test_norm_trans_2d);
}

DEFINE_MAIN;
