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
TARGET_LINK_LIBRARIES( rgrl_test_all rgrl rrel testlib vnl_algo vnl vil )

ADD_TEST( rgrl_rgrl_util              ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_rgrl_util )
ADD_TEST( rgrl_transformation         ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_transformation )
ADD_TEST( rgrl_trans_reader           ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_trans_reader ${CMAKE_CURRENT_SOURCE_DIR}/affine_xform.txt )
ADD_TEST( rgrl_feature                ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_feature )
ADD_TEST( rgrl_feature_region         ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_feature_region )
ADD_TEST( rgrl_feature_set            ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_feature_set )
ADD_TEST( rgrl_match_set              ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_match_set )
ADD_TEST( rgrl_spline                 ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_spline )
ADD_TEST( rgrl_estimator              ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_estimator )
ADD_TEST( rgrl_convergence            ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_convergence )
ADD_TEST( rgrl_scale_est              ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_scale_est )
ADD_TEST( rgrl_initializer            ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_initializer )
ADD_TEST( rgrl_set_of                 ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_set_of )
ADD_TEST( rgrl_initializer_ran_sam    ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_initializer_ran_sam )
ADD_TEST( rgrl_matcher                ${EXECUTABLE_OUTPUT_PATH}/rgrl_test_all test_matcher )

ADD_EXECUTABLE( rgrl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( rgrl_test_include rgrl )
