# bapl/tests/CMakeLists.txt

add_executable( bapl_test_all
  test_driver.cxx
  test_transform.cxx
  test_roi.cxx
  test_matcher.cxx
  test_lowe_pyramid_set.cxx
  test_compute_tracks.cxx
  test_match_keypoints.cxx
  test_dense_sift.cxx
)

target_link_libraries(bapl_test_all bapl ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl brip ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vul)

add_test( NAME bapl_test_transform        COMMAND $<TARGET_FILE:bapl_test_all> test_transform )
add_test( NAME bapl_test_roi              COMMAND $<TARGET_FILE:bapl_test_all> test_roi )
add_test( NAME bapl_test_matcher          COMMAND $<TARGET_FILE:bapl_test_all> test_matcher )
add_test( NAME bapl_test_lowe_pyramid_set COMMAND $<TARGET_FILE:bapl_test_all> test_lowe_pyramid_set )
add_test( NAME bapl_test_compute_tracks   COMMAND $<TARGET_FILE:bapl_test_all> test_compute_tracks ${CMAKE_CURRENT_SOURCE_DIR})
add_test( NAME bapl_test_match_keypoints  COMMAND $<TARGET_FILE:bapl_test_all> test_match_keypoints ${CMAKE_CURRENT_SOURCE_DIR})
add_test( NAME bapl_test_dense_sift       COMMAND $<TARGET_FILE:bapl_test_all> test_dense_sift ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable( bapl_test_include test_include.cxx )
target_link_libraries( bapl_test_include bapl )
add_executable( bapl_test_template_include test_template_include.cxx )
target_link_libraries( bapl_test_template_include bapl )
