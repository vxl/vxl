# core/vrel/tests/CMakeLists.txt

add_executable( vrel_test_all
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
target_link_libraries( vrel_test_all vrel ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}testlib )

add_test( NAME vrel_test_homography2d_est COMMAND $<TARGET_FILE:vrel_test_all> test_homography2d_est )
add_test( NAME vrel_test_shift2d_est COMMAND $<TARGET_FILE:vrel_test_all> test_shift2d_est )
add_test( NAME vrel_test_irls COMMAND $<TARGET_FILE:vrel_test_all> test_irls )
add_test( NAME vrel_test_linear_reg COMMAND $<TARGET_FILE:vrel_test_all> test_linear_reg )
add_test( NAME vrel_test_lms_lts COMMAND $<TARGET_FILE:vrel_test_all> test_lms_lts )
add_test( NAME vrel_test_m_est_obj COMMAND $<TARGET_FILE:vrel_test_all> test_m_est_obj )
add_test( NAME vrel_test_muse_table COMMAND $<TARGET_FILE:vrel_test_all> test_muse_table )
add_test( NAME vrel_test_orthogonal_regression COMMAND $<TARGET_FILE:vrel_test_all> test_orthogonal_regression )
add_test( NAME vrel_test_ran_sam_search COMMAND $<TARGET_FILE:vrel_test_all> test_ran_sam_search )
add_test( NAME vrel_test_ransac_obj COMMAND $<TARGET_FILE:vrel_test_all> test_ransac_obj )
add_test( NAME vrel_test_robust_util COMMAND $<TARGET_FILE:vrel_test_all> test_robust_util )
add_test( NAME vrel_test_similarity_from_matches COMMAND $<TARGET_FILE:vrel_test_all> test_similarity_from_matches )

add_executable( vrel_test_include test_include.cxx )
target_link_libraries( vrel_test_include vrel )
add_executable( vrel_test_template_include test_template_include.cxx )
target_link_libraries( vrel_test_template_include vrel )
