INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

ADD_EXECUTABLE( rgrl_test_all
  test_driver.cxx
  test_util.cxx            test_util.h
  
  test_rgrl_util.cxx
  test_transformation.cxx
  test_trans_reader.cxx
  test_feature.cxx
  test_feature_region.cxx
  test_feature_set.cxx
  test_match_set.cxx
  test_spline.cxx
  test_estimator.cxx
  test_convergence.cxx
  test_scale_est.cxx
  test_initializer.cxx
  test_set_of.cxx
  test_initializer_ran_sam.cxx
  test_matcher.cxx
)
TARGET_LINK_LIBRARIES( rgrl_test_all rgrl testlib vnl_algo vnl mbl )

ADD_TEST( rgrl_rgrl_util              rgrl_test_all test_rgrl_util )
ADD_TEST( rgrl_transformation         rgrl_test_all test_transformation )
ADD_TEST( rgrl_trans_reader           rgrl_test_all test_trans_reader ${CMAKE_CURRENT_SOURCE_DIR}/affine_xform.txt )
ADD_TEST( rgrl_feature                rgrl_test_all test_feature )
ADD_TEST( rgrl_feature_region         rgrl_test_all test_feature_region )
ADD_TEST( rgrl_feature_set            rgrl_test_all test_feature_set )
ADD_TEST( rgrl_match_set              rgrl_test_all test_match_set )
ADD_TEST( rgrl_spline                 rgrl_test_all test_spline )
ADD_TEST( rgrl_estimator              rgrl_test_all test_estimator )
ADD_TEST( rgrl_convergence            rgrl_test_all test_convergence )
ADD_TEST( rgrl_scale_est              rgrl_test_all test_scale_est )
ADD_TEST( rgrl_initializer            rgrl_test_all test_initializer )
ADD_TEST( rgrl_set_of                 rgrl_test_all test_set_of )
ADD_TEST( rgrl_initializer_ran_sam    rgrl_test_all test_initializer_ran_sam )
ADD_TEST( rgrl_matcher                rgrl_test_all test_matcher )

ADD_EXECUTABLE( rgrl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( rgrl_test_include rgrl )
