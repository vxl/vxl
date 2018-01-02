# This is core/vgl/algo/tests/CMakeLists.txt

add_executable( vgl_algo_test_all
  test_driver.cxx
  test_compute_similarity_3d.cxx
  test_compute_rigid_3d.cxx
  test_conic.cxx
  test_convex_hull_2d.cxx
  test_fit_conics_2d.cxx
  test_fit_lines_2d.cxx
  test_fit_plane_3d.cxx
  test_fit_sphere_3d.cxx
  test_fit_quadric_3d.cxx
  test_h_matrix_1d.cxx
  test_h_matrix_2d.cxx
  test_h_matrix_3d.cxx
  test_homg.cxx
  test_intersection.cxx
  test_orient_box_3d.cxx
  test_p_matrix.cxx
  test_rotation_3d.cxx
  test_rtree.cxx
)
target_link_libraries( vgl_algo_test_all ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}testlib )

add_test( NAME vgl_test_compute_similarity_3d COMMAND $<TARGET_FILE:vgl_algo_test_all> test_compute_similarity_3d )
add_test( NAME vgl_test_compute_rigid_3d COMMAND $<TARGET_FILE:vgl_algo_test_all> test_compute_rigid_3d )
add_test( NAME vgl_orient_box_3d COMMAND $<TARGET_FILE:vgl_algo_test_all> test_orient_box_3d )
add_test( NAME vgl_test_conic COMMAND $<TARGET_FILE:vgl_algo_test_all> test_conic )
add_test( NAME vgl_test_convex_hull_2d COMMAND $<TARGET_FILE:vgl_algo_test_all> test_convex_hull_2d)
add_test( NAME vgl_test_fit_conics_2d COMMAND $<TARGET_FILE:vgl_algo_test_all> test_fit_conics_2d)
add_test( NAME vgl_test_fit_lines_2d COMMAND $<TARGET_FILE:vgl_algo_test_all> test_fit_lines_2d)
add_test( NAME vgl_test_fit_plane_3d COMMAND $<TARGET_FILE:vgl_algo_test_all> test_fit_plane_3d)
add_test( NAME vgl_test_fit_sphere_3d COMMAND $<TARGET_FILE:vgl_algo_test_all> test_fit_plane_3d)
add_test( NAME vgl_test_fit_quadric_3d COMMAND $<TARGET_FILE:vgl_algo_test_all> test_fit_plane_3d)
add_test( NAME vgl_test_h_matrix_1d COMMAND $<TARGET_FILE:vgl_algo_test_all> test_h_matrix_1d )
add_test( NAME vgl_test_h_matrix_2d COMMAND $<TARGET_FILE:vgl_algo_test_all> test_h_matrix_2d )
add_test( NAME vgl_test_h_matrix_3d COMMAND $<TARGET_FILE:vgl_algo_test_all> test_h_matrix_3d )
add_test( NAME vgl_test_homg COMMAND $<TARGET_FILE:vgl_algo_test_all> test_homg )
add_test( NAME vgl_test_intersection COMMAND $<TARGET_FILE:vgl_algo_test_all> test_intersection)
add_test( NAME vgl_test_p_matrix COMMAND $<TARGET_FILE:vgl_algo_test_all> test_p_matrix)
add_test( NAME vgl_test_rotation_3d COMMAND $<TARGET_FILE:vgl_algo_test_all> test_rotation_3d)
add_test( NAME vgl_test_rtree COMMAND $<TARGET_FILE:vgl_algo_test_all> test_rtree)

add_executable( vgl_algo_test_include test_include.cxx )
target_link_libraries( vgl_algo_test_include ${VXL_LIB_PREFIX}vgl_algo)

add_executable( vgl_algo_test_template_include test_template_include.cxx )
target_link_libraries( vgl_algo_test_template_include ${VXL_LIB_PREFIX}vgl_algo)
