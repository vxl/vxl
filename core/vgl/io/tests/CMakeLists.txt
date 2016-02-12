# This is core/vgl/io/tests/CMakeLists.txt

add_executable( vgl_io_test_all
  test_driver.cxx

  test_point_2d_io.cxx
  test_point_3d_io.cxx
  test_homg_point_1d_io.cxx
  test_homg_point_2d_io.cxx
  test_homg_point_3d_io.cxx
  test_vector_2d_io.cxx
  test_vector_3d_io.cxx
  test_line_2d_io.cxx
  test_line_3d_2_points_io.cxx
  test_line_segment_2d_io.cxx
  test_line_segment_3d_io.cxx
  test_homg_line_2d_io.cxx
  test_homg_line_3d_2_points_io.cxx
  test_homg_plane_3d_io.cxx
  test_conic_io.cxx
  test_box_2d_io.cxx
  test_box_3d_io.cxx
  test_plane_3d_io.cxx
  test_polygon_io.cxx
  golden_test_vgl_io.cxx
)
target_link_libraries( vgl_io_test_all ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vpl )

add_test( NAME vgl_io_test_point_2d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_point_2d_io )
add_test( NAME vgl_io_test_point_3d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_point_3d_io )
add_test( NAME vgl_io_test_homg_point_1d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_homg_point_1d_io )
add_test( NAME vgl_io_test_homg_point_2d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_homg_point_2d_io )
add_test( NAME vgl_io_test_homg_point_3d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_homg_point_3d_io )
add_test( NAME vgl_io_test_vector_2d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_vector_2d_io )
add_test( NAME vgl_io_test_vector_3d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_vector_3d_io )
add_test( NAME vgl_io_test_line_2d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_line_2d_io )
add_test( NAME vgl_io_test_line_3d_2_points_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_line_3d_2_points_io )
add_test( NAME vgl_io_test_line_segment_2d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_line_segment_2d_io )
add_test( NAME vgl_io_test_line_segment_3d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_line_segment_3d_io )
add_test( NAME vgl_io_test_homg_line_2d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_homg_line_2d_io )
add_test( NAME vgl_io_test_homg_line_3d_2_points_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_homg_line_3d_2_points_io )
add_test( NAME vgl_io_test_homg_plane_3d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_homg_plane_3d_io )
add_test( NAME vgl_io_test_conic_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_conic_io )
add_test( NAME vgl_io_test_box_2d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_box_2d_io )
add_test( NAME vgl_io_test_box_3d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_box_3d_io )
add_test( NAME vgl_io_test_plane_3d_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_plane_3d_io )
add_test( NAME vgl_io_test_polygon_io COMMAND $<TARGET_FILE:vgl_io_test_all> test_polygon_io )
add_test( NAME vgl_io_golden_test COMMAND $<TARGET_FILE:vgl_io_test_all> golden_test_vgl_io )

add_executable( vgl_io_test_include test_include.cxx )
target_link_libraries( vgl_io_test_include ${VXL_LIB_PREFIX}vgl_io )
add_executable( vgl_io_test_template_include test_template_include.cxx )
target_link_libraries( vgl_io_test_template_include ${VXL_LIB_PREFIX}vgl_io )
