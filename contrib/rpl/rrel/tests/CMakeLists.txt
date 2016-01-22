# rpl/rrel/tests/CMakeLists.txt

ADD_EXECUTABLE( rrel_test_all
  test_driver.cxx

  # Supporting class
  similarity_from_matches.h  similarity_from_matches.cxx

  # The tests
  test_homography2d_est.cxx
  test_shift2d_est.cxx
  test_irls.cxx
  test_linear_reg.cxx
  test_lms_lts.cxx
  test_m_est_obj.cxx
  test_muse_table.cxx
  test_orthogonal_regression.cxx
  test_ran_sam_search.cxx
  test_ransac_obj.cxx
  test_robust_util.cxx
  test_similarity_from_matches.cxx
)
TARGET_LINK_LIBRARIES( rrel_test_all rrel vnl_algo vnl testlib )

ADD_TEST( rrel_test_homography2d_est         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rrel_test_all test_homography2d_est )
ADD_TEST( rrel_test_shift2d_est              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rrel_test_all test_shift2d_est )
ADD_TEST( rrel_test_irls                     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rrel_test_all test_irls )
ADD_TEST( rrel_test_linear_reg               ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rrel_test_all test_linear_reg )
ADD_TEST( rrel_test_lms_lts                  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rrel_test_all test_lms_lts )
ADD_TEST( rrel_test_m_est_obj                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rrel_test_all test_m_est_obj )
ADD_TEST( rrel_test_muse_table               ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rrel_test_all test_muse_table )
ADD_TEST( rrel_test_orthogonal_regression    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rrel_test_all test_orthogonal_regression )
ADD_TEST( rrel_test_ran_sam_search           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rrel_test_all test_ran_sam_search )
ADD_TEST( rrel_test_ransac_obj               ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rrel_test_all test_ransac_obj )
ADD_TEST( rrel_test_robust_util              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rrel_test_all test_robust_util )
ADD_TEST( rrel_test_similarity_from_matches  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rrel_test_all test_similarity_from_matches )

ADD_EXECUTABLE( rrel_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( rrel_test_include rrel )
ADD_EXECUTABLE( rrel_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( rrel_test_template_include rrel )
