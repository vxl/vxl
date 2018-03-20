# This is core/vgl/tests/CMakeLists.txt

add_executable( vgl_test_all
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
  test_quadric.cxx
  test_oriented_box_2d.cxx
  test_affine_coordinates.cxx
)
target_link_libraries( vgl_test_all ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}testlib )

add_test( NAME vgl_test_cartesian COMMAND $<TARGET_FILE:vgl_test_all> test_cartesian )
add_test( NAME vgl_test_distance COMMAND $<TARGET_FILE:vgl_test_all> test_distance )
add_test( NAME vgl_test_closest_point COMMAND $<TARGET_FILE:vgl_test_all> test_closest_point )
add_test( NAME vgl_test_polygon COMMAND $<TARGET_FILE:vgl_test_all> test_polygon )
add_test( NAME vgl_test_convex COMMAND $<TARGET_FILE:vgl_test_all> test_convex )
add_test( NAME vgl_test_polygon_scan_iterator COMMAND $<TARGET_FILE:vgl_test_all> test_polygon_scan_iterator )
add_test( NAME vgl_test_triangle_scan_iterator COMMAND $<TARGET_FILE:vgl_test_all> test_triangle_scan_iterator )
add_test( NAME vgl_test_ellipse_scan_iterator COMMAND $<TARGET_FILE:vgl_test_all> test_ellipse_scan_iterator )
add_test( NAME vgl_test_window_scan_iterator COMMAND $<TARGET_FILE:vgl_test_all> test_window_scan_iterator )
add_test( NAME vgl_test_area COMMAND $<TARGET_FILE:vgl_test_all> test_area )
add_test( NAME vgl_test_clip COMMAND $<TARGET_FILE:vgl_test_all> test_clip )
add_test( NAME vgl_test_sphere COMMAND $<TARGET_FILE:vgl_test_all> test_sphere)
add_test( NAME vgl_test_line_3d_2_points COMMAND $<TARGET_FILE:vgl_test_all> test_line_3d_2_points)
add_test( NAME vgl_test_line_segment_3d COMMAND $<TARGET_FILE:vgl_test_all> test_line_segment_3d)
add_test( NAME vgl_test_triangle_3d COMMAND $<TARGET_FILE:vgl_test_all> test_triangle_3d)
add_test( NAME vgl_test_triangle_3d_line_intersection COMMAND $<TARGET_FILE:vgl_test_all> test_triangle_3d_line_intersection)
add_test( NAME vgl_test_infinite_line_3d COMMAND $<TARGET_FILE:vgl_test_all> test_infinite_line_3d)
add_test( NAME vgl_test_ray_3d COMMAND $<TARGET_FILE:vgl_test_all> test_ray_3d)
add_test( NAME vgl_test_plane_3d COMMAND $<TARGET_FILE:vgl_test_all> test_plane_3d)
add_test( NAME vgl_test_frustum_3d COMMAND $<TARGET_FILE:vgl_test_all> test_frustum_3d)
add_test( NAME vgl_test_intersection COMMAND $<TARGET_FILE:vgl_test_all> test_intersection)
add_test( NAME vgl_test_spline COMMAND $<TARGET_FILE:vgl_test_all> test_spline)
add_test( NAME vgl_test_pointset COMMAND $<TARGET_FILE:vgl_test_all> test_pointset)
add_test( NAME vgl_test_quadric COMMAND $<TARGET_FILE:vgl_test_all> test_quadric)
add_test( NAME vgl_test_oriented_box_2d COMMAND $<TARGET_FILE:vgl_test_all> test_oriented_box_2d)
add_test( NAME vgl_test_affine_coordinates COMMAND $<TARGET_FILE:vgl_test_all> test_affine_coordinates)
add_executable( vgl_test_include test_include.cxx )
target_link_libraries( vgl_test_include ${VXL_LIB_PREFIX}vgl )
add_executable( vgl_test_template_include test_template_include.cxx )
target_link_libraries( vgl_test_template_include ${VXL_LIB_PREFIX}vgl )
