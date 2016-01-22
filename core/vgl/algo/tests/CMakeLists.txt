# This is core/vgl/algo/tests/CMakeLists.txt

ADD_EXECUTABLE( vgl_algo_test_all
  test_driver.cxx

  test_compute_similarity_3d.cxx
  test_conic.cxx
  test_convex_hull_2d.cxx
  test_ellipsoid.cxx
  test_fit_conics_2d.cxx
  test_fit_lines_2d.cxx
  test_fit_plane_3d.cxx
  test_fit_sphere_3d.cxx
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
TARGET_LINK_LIBRARIES( vgl_algo_test_all vgl_algo testlib )

ADD_TEST( vgl_test_compute_similarity_3d          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_compute_similarity_3d )
ADD_TEST( vgl_test_conic                          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_conic )
ADD_TEST( vgl_test_convex_hull_2d                 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_convex_hull_2d)
ADD_TEST( vgl_test_ellipsoid                      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_ellipsoid)
ADD_TEST( vgl_test_fit_conics_2d                  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_fit_conics_2d)
ADD_TEST( vgl_test_fit_lines_2d                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_fit_lines_2d)
ADD_TEST( vgl_test_fit_plane_3d                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_fit_plane_3d)
ADD_TEST( vgl_test_fit_sphere_3d                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_fit_plane_3d)
ADD_TEST( vgl_test_h_matrix_1d                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_h_matrix_1d )
ADD_TEST( vgl_test_h_matrix_2d                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_h_matrix_2d )
ADD_TEST( vgl_test_h_matrix_3d                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_h_matrix_3d )
ADD_TEST( vgl_test_homg                           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_homg )
ADD_TEST( vgl_test_intersection                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_intersection)
ADD_TEST( vgl_test_orient_box_3d                  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_orient_box_3d)
ADD_TEST( vgl_test_p_matrix                       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_p_matrix)
ADD_TEST( vgl_test_rotation_3d                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_rotation_3d)
ADD_TEST( vgl_test_rtree                          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgl_algo_test_all test_rtree)

ADD_EXECUTABLE( vgl_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vgl_algo_test_include vgl_algo)

ADD_EXECUTABLE( vgl_algo_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vgl_algo_test_template_include vgl_algo)
