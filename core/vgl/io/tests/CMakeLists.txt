# This is core/vgl/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vgl_io_test_all
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
TARGET_LINK_LIBRARIES( vgl_io_test_all vgl_io testlib vpl )

ADD_TEST( vgl_io_test_point_2d_io        vgl_io_test_all test_point_2d_io )
ADD_TEST( vgl_io_test_point_3d_io        vgl_io_test_all test_point_3d_io )
ADD_TEST( vgl_io_test_homg_point_1d_io   vgl_io_test_all test_homg_point_1d_io )
ADD_TEST( vgl_io_test_homg_point_2d_io   vgl_io_test_all test_homg_point_2d_io )
ADD_TEST( vgl_io_test_homg_point_3d_io   vgl_io_test_all test_homg_point_3d_io )
ADD_TEST( vgl_io_test_vector_2d_io       vgl_io_test_all test_vector_2d_io )
ADD_TEST( vgl_io_test_vector_3d_io       vgl_io_test_all test_vector_3d_io )
ADD_TEST( vgl_io_test_line_2d_io         vgl_io_test_all test_line_2d_io )
ADD_TEST( vgl_io_test_line_3d_2_points_io vgl_io_test_all test_line_3d_2_points_io )
ADD_TEST( vgl_io_test_line_segment_2d_io vgl_io_test_all test_line_segment_2d_io )
ADD_TEST( vgl_io_test_line_segment_3d_io vgl_io_test_all test_line_segment_3d_io )
ADD_TEST( vgl_io_test_homg_line_2d_io    vgl_io_test_all test_homg_line_2d_io )
ADD_TEST( vgl_io_test_homg_line_3d_2_points_io vgl_io_test_all test_homg_line_3d_2_points_io )
ADD_TEST( vgl_io_test_homg_plane_3d_io   vgl_io_test_all test_homg_plane_3d_io )
ADD_TEST( vgl_io_test_conic_io           vgl_io_test_all test_conic_io )
ADD_TEST( vgl_io_test_box_2d_io          vgl_io_test_all test_box_2d_io )
ADD_TEST( vgl_io_test_box_3d_io          vgl_io_test_all test_box_3d_io )
ADD_TEST( vgl_io_test_plane_3d_io        vgl_io_test_all test_plane_3d_io )
ADD_TEST( vgl_io_test_polygon_io         vgl_io_test_all test_polygon_io )
ADD_TEST( vgl_io_golden_test             vgl_io_test_all golden_test_vgl_io )

ADD_EXECUTABLE( vgl_io_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vgl_io_test_include vgl_io )
