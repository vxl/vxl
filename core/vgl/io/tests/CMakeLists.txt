# ./vxl/vgl/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vgl_io_test_driver
  test_driver.cxx

  test_includes.cxx
  test_point_2d_io.cxx
  test_point_3d_io.cxx
  test_homg_point_2d_io.cxx
  test_homg_point_3d_io.cxx
  test_vector_2d_io.cxx
  test_vector_3d_io.cxx
  test_line_2d_io.cxx
  test_line_segment_2d_io.cxx
  test_line_segment_3d_io.cxx
  test_homg_line_2d_io.cxx
  test_homg_line_3d_2_points_io.cxx
  test_homg_plane_3d_io.cxx
  test_conic_io.cxx
  test_box_2d_io.cxx
  test_box_3d_io.cxx
  test_polygon_io.cxx
  golden_test_vgl_io.cxx
)

ADD_TEST( vgl_io_test_includes           vgl_io_test_driver test_includes )
ADD_TEST( vgl_io_test_point_2d_io        vgl_io_test_driver test_point_2d_io )
ADD_TEST( vgl_io_test_point_3d_io        vgl_io_test_driver test_point_3d_io )
ADD_TEST( vgl_io_test_homg_point_2d_io   vgl_io_test_driver test_homg_point_2d_io )
ADD_TEST( vgl_io_test_homg_point_3d_io   vgl_io_test_driver test_homg_point_3d_io )
ADD_TEST( vgl_io_test_vector_2d_io       vgl_io_test_driver test_vector_2d_io )
ADD_TEST( vgl_io_test_vector_3d_io       vgl_io_test_driver test_vector_3d_io )
ADD_TEST( vgl_io_test_line_2d_io         vgl_io_test_driver test_line_2d_io )
ADD_TEST( vgl_io_test_line_segment_2d_io vgl_io_test_driver test_line_segment_2d_io )
ADD_TEST( vgl_io_test_line_segment_3d_io vgl_io_test_driver test_line_segment_3d_io )
ADD_TEST( vgl_io_test_homg_line_2d_io    vgl_io_test_driver test_homg_line_2d_io )
ADD_TEST( vgl_io_test_homg_line_3d_2_points_io vgl_io_test_driver test_homg_line_3d_2_points_io )
ADD_TEST( vgl_io_test_homg_plane_3d_io   vgl_io_test_driver test_homg_plane_3d_io )
ADD_TEST( vgl_io_test_conic_io           vgl_io_test_driver test_conic_io )
ADD_TEST( vgl_io_test_box_2d_io          vgl_io_test_driver test_box_2d_io )
ADD_TEST( vgl_io_test_box_3d_io          vgl_io_test_driver test_box_3d_io )
ADD_TEST( vgl_io_test_polygon_io         vgl_io_test_driver test_polygon_io )
ADD_TEST( vgl_io_golden_test             vgl_io_test_driver golden_test_vgl_io )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/io/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/testlib/CMakeListsLink.txt )
