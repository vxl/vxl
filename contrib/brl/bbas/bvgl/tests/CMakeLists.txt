# brl/bbas/bvgl/tests/CMakeLists.txt

LINK_LIBRARIES(bvgl vgl vnl testlib)

ADD_EXECUTABLE(bvgl_test_all
  test_driver.cxx
  bvgl_test_h_matrix_1d.cxx
  bvgl_test_h_matrix_2d.cxx
  bvgl_test_h_matrix_3d.cxx
  bvgl_test_norm_trans_2d.cxx
  bvgl_test_h_matrix_2d_compute_linear.cxx
  bvgl_test_h_matrix_2d_compute_4point.cxx
)

ADD_TEST(bvgl_test_h_matrix_1d                bvgl_test_all bvgl_test_h_matrix_1d)
ADD_TEST(bvgl_test_h_matrix_2d                bvgl_test_all bvgl_test_h_matrix_2d)
ADD_TEST(bvgl_test_h_matrix_3d                bvgl_test_all bvgl_test_h_matrix_3d)
ADD_TEST(bvgl_test_norm_trans_2d              bvgl_test_all bvgl_test_norm_trans_2d)
ADD_TEST(bvgl_test_h_matrix_2d_compute_linear bvgl_test_all bvgl_test_h_matrix_2d_compute_linear)
ADD_TEST(bvgl_test_h_matrix_2d_compute_4point bvgl_test_all bvgl_test_h_matrix_2d_compute_4point)

ADD_EXECUTABLE(bvgl_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(bvgl_test_include bvgl)
