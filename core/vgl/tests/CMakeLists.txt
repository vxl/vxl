# This is core/vgl/tests/CMakeLists.txt

ADD_EXECUTABLE( vgl_test_all
  test_driver.cxx

  test_cartesian.cxx
  test_distance.cxx
  test_closest_point.cxx
  test_conic.cxx
  test_homg.cxx
  test_polygon.cxx
  test_convex.cxx
  test_polygon_scan_iterator.cxx
  test_triangle_scan_iterator.cxx
  test_ellipse_scan_iterator.cxx
  test_window_scan_iterator.cxx
  test_area.cxx
  test_clip.cxx
  test_h_matrix_1d.cxx
  test_h_matrix_2d.cxx
  test_h_matrix_3d.cxx
  test_fit_lines_2d.cxx
  test_p_matrix.cxx
)
TARGET_LINK_LIBRARIES( vgl_test_all vgl_algo vgl testlib )

ADD_TEST( vgl_test_cartesian                    vgl_test_all test_cartesian )
ADD_TEST( vgl_test_distance                     vgl_test_all test_distance )
ADD_TEST( vgl_test_closest_point                vgl_test_all test_closest_point )
ADD_TEST( vgl_test_conic                        vgl_test_all test_conic )
ADD_TEST( vgl_test_homg                         vgl_test_all test_homg )
ADD_TEST( vgl_test_polygon                      vgl_test_all test_polygon )
ADD_TEST( vgl_test_convex                       vgl_test_all test_convex )
ADD_TEST( vgl_test_polygon_scan_iterator        vgl_test_all test_polygon_scan_iterator )
ADD_TEST( vgl_test_triangle_scan_iterator       vgl_test_all test_triangle_scan_iterator )
ADD_TEST( vgl_test_ellipse_scan_iterator        vgl_test_all test_ellipse_scan_iterator )
ADD_TEST( vgl_test_window_scan_iterator         vgl_test_all test_window_scan_iterator )
ADD_TEST( vgl_test_area                         vgl_test_all test_area )
ADD_TEST( vgl_test_clip                         vgl_test_all test_clip )
ADD_TEST( vgl_test_h_matrix_1d                  vgl_test_all test_h_matrix_1d )
ADD_TEST( vgl_test_h_matrix_2d                  vgl_test_all test_h_matrix_2d )
ADD_TEST( vgl_test_h_matrix_3d                  vgl_test_all test_h_matrix_3d )
ADD_TEST( vgl_test_fit_lines_2d                 vgl_test_all test_fit_lines_2d)
ADD_TEST( vgl_test_p_matrix                     vgl_test_all test_p_matrix)

ADD_EXECUTABLE( vgl_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( vgl_test_include vgl_algo vgl )
