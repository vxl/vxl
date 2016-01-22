# This is mul/m23d/tests/CMakeLists.txt

ADD_EXECUTABLE( m23d_test_all
  test_driver.cxx

  test_make_ortho_projection.cxx
  test_rotation_from_ortho_projection.cxx
  test_ortho_rigid_builder.cxx
  test_ortho_flexible_builder.cxx
  test_rotation_matrix.cxx
  test_correction_matrix_error.cxx
)
TARGET_LINK_LIBRARIES( m23d_test_all m23d testlib )

ADD_TEST( NAME m23d_test_make_ortho_projection COMMAND $<TARGET_FILE:m23d_test_all> test_make_ortho_projection )
ADD_TEST( NAME m23d_test_rotation_from_ortho_projection COMMAND $<TARGET_FILE:m23d_test_all> test_rotation_from_ortho_projection )
ADD_TEST( NAME m23d_test_ortho_rigid_builder COMMAND $<TARGET_FILE:m23d_test_all> test_ortho_rigid_builder )
ADD_TEST( NAME m23d_test_ortho_flexible_builder COMMAND $<TARGET_FILE:m23d_test_all> test_ortho_flexible_builder )
ADD_TEST( NAME m23d_test_rotation_matrix COMMAND $<TARGET_FILE:m23d_test_all> test_rotation_matrix )
ADD_TEST( NAME m23d_test_correction_matrix_error COMMAND $<TARGET_FILE:m23d_test_all> test_correction_matrix_error )

ADD_EXECUTABLE( m23d_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( m23d_test_include m23d )
