# brl/bbas/bvgl/tests/CMakeLists.txt

LINK_LIBRARIES(bvgl vgl vnl )

ADD_EXECUTABLE(bvgl_test_h_matrix_1d  bvgl_test_h_matrix_1d.cxx)
ADD_TEST(bvgl_test_h_matrix_1d bvgl_test_h_matrix_1d)

ADD_EXECUTABLE(bvgl_test_h_matrix_2d  bvgl_test_h_matrix_2d.cxx)
ADD_TEST(bvgl_test_h_matrix_2d bvgl_test_h_matrix_2d)

ADD_EXECUTABLE(bvgl_test_h_matrix_3d  bvgl_test_h_matrix_3d.cxx)
ADD_TEST(bvgl_test_h_matrix_3d bvgl_test_h_matrix_3d)

