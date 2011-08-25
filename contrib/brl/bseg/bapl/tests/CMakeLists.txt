# bapl/tests/CMakeLists.txt

ADD_EXECUTABLE( bapl_test_all
  test_driver.cxx
  test_transform.cxx
  test_roi.cxx
  test_matcher.cxx
  test_lowe_pyramid_set.cxx
  test_compute_tracks.cxx
  test_match_keypoints.cxx 
  test_dense_sift.cxx
)

TARGET_LINK_LIBRARIES(bapl_test_all bapl testlib vil vnl brip vpgl_algo vgl_algo vul)

ADD_TEST( bapl_test_transform         bapl_test_all test_transform )
ADD_TEST( bapl_test_roi               bapl_test_all test_roi )
ADD_TEST( bapl_test_matcher           bapl_test_all test_matcher )
ADD_TEST( bapl_test_lowe_pyramid_set  bapl_test_all test_lowe_pyramid_set )
ADD_TEST( bapl_test_compute_tracks    bapl_test_all test_compute_tracks ${CMAKE_CURRENT_SOURCE_DIR})
ADD_TEST( bapl_test_match_keypoints   bapl_test_all test_match_keypoints ${CMAKE_CURRENT_SOURCE_DIR})
ADD_TEST( bapl_test_dense_sift        bapl_test_all test_dense_sift ${CMAKE_CURRENT_SOURCE_DIR} )

ADD_EXECUTABLE( bapl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bapl_test_include bapl )
ADD_EXECUTABLE( bapl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bapl_test_template_include bapl )
