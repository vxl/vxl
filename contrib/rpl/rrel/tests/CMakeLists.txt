# ./rpl/rrel/tests/CMakeLists.txt

ADD_LIBRARY( rrel_tests rrel_tests_sources )

SOURCE_FILES( rrel_tests_sources similarity_from_matches.cxx )

AUX_SOURCE_DIRECTORY( Templates rrel_tests_sources )

LINK_LIBRARIES( rrel_tests )

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/rpl/rrel/tests/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

ADD_EXECUTABLE( rrel_test_link                  test_link.cxx )
ADD_EXECUTABLE( rrel_test_m_est_obj             test_m_est_obj.cxx )
ADD_EXECUTABLE( rrel_test_ransac_obj            test_ransac_obj.cxx )
ADD_EXECUTABLE( rrel_test_lms_lts               test_lms_lts.cxx )
ADD_EXECUTABLE( rrel_test_irls                  test_irls.cxx )
ADD_EXECUTABLE( rrel_test_ran_sam_search        test_ran_sam_search.cxx )

ADD_EXECUTABLE( rrel_test_linear_reg            test_linear_reg.cxx )
ADD_EXECUTABLE( rrel_test_orthogonal_regression test_orthogonal_regression.cxx )
ADD_EXECUTABLE( rrel_test_homography2d_est      test_homography2d_est.cxx )

ADD_EXECUTABLE( rrel_test_robust_util           test_robust_util.cxx )

ADD_TEST( rrel_test_link rrel_test_link )
ADD_TEST( rrel_test_m_est_obj rrel_test_m_est_obj )
ADD_TEST( rrel_test_ransac_obj rrel_test_ransac_obj )
ADD_TEST( rrel_test_lms_lts rrel_test_lms_lts )
ADD_TEST( rrel_test_irls rrel_test_irls )
ADD_TEST( rrel_test_ran_sam_search rrel_test_ran_sam_search )

ADD_TEST( rrel_test_linear_reg rrel_test_linear_reg )
ADD_TEST( rrel_test_orthogonal_regression rrel_test_orthogonal_regression )
ADD_TEST( rrel_test_homography2d_est rrel_test_homography2d_est )

ADD_TEST( rrel_test_robust_util rrel_test_robust_util )


INCLUDE( ${allvxl_SOURCE_DIR}/rpl/rrel/CMakeListsLink.txt )
