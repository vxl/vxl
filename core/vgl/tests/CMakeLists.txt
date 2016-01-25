# This is core/vgl/tests/CMakeLists.txt

ADD_EXECUTABLE( vgl_test_all
  test_driver.cxx

  test_cartesian.cxx
  test_distance.cxx
  test_closest_point.cxx
  test_polygon.cxx
  test_convex.cxx
  test_polygon_scan_iterator.cxx
  test_triangle_scan_iterator.cxx
  test_ellipse_scan_iterator.cxx
  test_window_scan_iterator.cxx
  test_area.cxx
  test_clip.cxx
  test_sphere.cxx
  test_line_3d_2_points.cxx
  test_line_segment_3d.cxx
  test_triangle_3d.cxx
  test_triangle_3d_line_intersection.cxx
  test_infinite_line_3d.cxx
  test_ray_3d.cxx
  test_plane_3d.cxx
  test_frustum_3d.cxx
  test_intersection.cxx
  test_spline.cxx
  test_pointset.cxx
)
TARGET_LINK_LIBRARIES( vgl_test_all vgl vpl testlib )

ADD_TEST( NAME vgl_test_cartesian COMMAND $<TARGET_FILE:vgl_test_all> test_cartesian )
ADD_TEST( NAME vgl_test_distance COMMAND $<TARGET_FILE:vgl_test_all> test_distance )
ADD_TEST( NAME vgl_test_closest_point COMMAND $<TARGET_FILE:vgl_test_all> test_closest_point )
ADD_TEST( NAME vgl_test_polygon COMMAND $<TARGET_FILE:vgl_test_all> test_polygon )
ADD_TEST( NAME vgl_test_convex COMMAND $<TARGET_FILE:vgl_test_all> test_convex )
ADD_TEST( NAME vgl_test_polygon_scan_iterator COMMAND $<TARGET_FILE:vgl_test_all> test_polygon_scan_iterator )
ADD_TEST( NAME vgl_test_triangle_scan_iterator COMMAND $<TARGET_FILE:vgl_test_all> test_triangle_scan_iterator )
ADD_TEST( NAME vgl_test_ellipse_scan_iterator COMMAND $<TARGET_FILE:vgl_test_all> test_ellipse_scan_iterator )
ADD_TEST( NAME vgl_test_window_scan_iterator COMMAND $<TARGET_FILE:vgl_test_all> test_window_scan_iterator )
ADD_TEST( NAME vgl_test_area COMMAND $<TARGET_FILE:vgl_test_all> test_area )
ADD_TEST( NAME vgl_test_clip COMMAND $<TARGET_FILE:vgl_test_all> test_clip )
ADD_TEST( NAME vgl_test_sphere COMMAND $<TARGET_FILE:vgl_test_all> test_sphere)
ADD_TEST( NAME vgl_test_line_3d_2_points COMMAND $<TARGET_FILE:vgl_test_all> test_line_3d_2_points)
ADD_TEST( NAME vgl_test_line_segment_3d COMMAND $<TARGET_FILE:vgl_test_all> test_line_segment_3d)
ADD_TEST( NAME vgl_test_triangle_3d COMMAND $<TARGET_FILE:vgl_test_all> test_triangle_3d)
ADD_TEST( NAME vgl_test_triangle_3d_line_intersection COMMAND $<TARGET_FILE:vgl_test_all> test_triangle_3d_line_intersection)
ADD_TEST( NAME vgl_test_infinite_line_3d COMMAND $<TARGET_FILE:vgl_test_all> test_infinite_line_3d)
ADD_TEST( NAME vgl_test_ray_3d COMMAND $<TARGET_FILE:vgl_test_all> test_ray_3d)
ADD_TEST( NAME vgl_test_plane_3d COMMAND $<TARGET_FILE:vgl_test_all> test_plane_3d)
ADD_TEST( NAME vgl_test_frustum_3d COMMAND $<TARGET_FILE:vgl_test_all> test_frustum_3d)
ADD_TEST( NAME vgl_test_intersection COMMAND $<TARGET_FILE:vgl_test_all> test_intersection)
ADD_TEST( NAME vgl_test_spline COMMAND $<TARGET_FILE:vgl_test_all> test_spline)
ADD_TEST( NAME vgl_test_pointset COMMAND $<TARGET_FILE:vgl_test_all> test_pointset)
ADD_EXECUTABLE( vgl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vgl_test_include vgl )
ADD_EXECUTABLE( vgl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vgl_test_template_include vgl )
