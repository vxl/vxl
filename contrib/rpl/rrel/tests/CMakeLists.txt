INCLUDE( ${allvxl_SOURCE_DIR}/rpl/rrel/CMakeListsLink.txt )

ADD_EXECUTABLE( test_link           test_link )
ADD_EXECUTABLE( test_m_est_obj      test_m_est_obj )
ADD_EXECUTABLE( test_ransac_obj     test_ransac_obj )
ADD_EXECUTABLE( test_lms_lts        test_lms_lts )
ADD_EXECUTABLE( test_irls           test_irls )
ADD_EXECUTABLE( test_ran_sam_search test_ran_sam_search )

ADD_EXECUTABLE( test_linear_reg     test_linear_reg )
ADD_EXECUTABLE( test_orthogonal_regression     test_orthogonal_regression )
ADD_EXECUTABLE( test_homography2d_est     test_homography2d_est )

ADD_EXECUTABLE( test_robust_util    test_robust_util )



ADD_TEST( test_link           test_link )
ADD_TEST( test_m_est_obj      test_m_est_obj )
ADD_TEST( test_ransac_obj     test_ransac_obj )
ADD_TEST( test_lms_lts        test_lms_lts )
ADD_TEST( test_irls           test_irls )
ADD_TEST( test_ran_sam_search test_ran_sam_search )

ADD_TEST( test_linear_reg     test_linear_reg )
ADD_TEST( test_orthogonal_regression     test_orthogonal_regression )
ADD_TEST( test_homography2d_est     test_homography2d_est )

ADD_TEST( test_robust_util    test_robust_util )
