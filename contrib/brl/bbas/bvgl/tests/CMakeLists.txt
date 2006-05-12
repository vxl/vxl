# brl/bbas/bvgl/tests/CMakeLists.txt

ADD_EXECUTABLE(bvgl_test_all
  test_driver.cxx
  bvgl_test_h_matrix_1d.cxx
  bvgl_test_h_matrix_2d.cxx
  bvgl_test_h_matrix_3d.cxx
  bvgl_test_norm_trans_2d.cxx
  bvgl_test_h_matrix_2d_compute_linear.cxx
  bvgl_test_h_matrix_2d_compute_4point.cxx
)
TARGET_LINK_LIBRARIES(bvgl_test_all bvgl vgl vnl testlib)

ADD_TEST(bvgl_test_h_matrix_1d                ${EXECUTABLE_OUTPUT_PATH}/bvgl_test_all bvgl_test_h_matrix_1d)
ADD_TEST(bvgl_test_h_matrix_2d                ${EXECUTABLE_OUTPUT_PATH}/bvgl_test_all bvgl_test_h_matrix_2d)
ADD_TEST(bvgl_test_h_matrix_3d                ${EXECUTABLE_OUTPUT_PATH}/bvgl_test_all bvgl_test_h_matrix_3d)
ADD_TEST(bvgl_test_norm_trans_2d              ${EXECUTABLE_OUTPUT_PATH}/bvgl_test_all bvgl_test_norm_trans_2d)
ADD_TEST(bvgl_test_h_matrix_2d_compute_linear ${EXECUTABLE_OUTPUT_PATH}/bvgl_test_all bvgl_test_h_matrix_2d_compute_linear)
ADD_TEST(bvgl_test_h_matrix_2d_compute_4point ${EXECUTABLE_OUTPUT_PATH}/bvgl_test_all bvgl_test_h_matrix_2d_compute_4point)

ADD_EXECUTABLE(bvgl_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(bvgl_test_include bvgl)
