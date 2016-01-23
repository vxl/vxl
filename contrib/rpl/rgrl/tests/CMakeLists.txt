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
TARGET_LINK_LIBRARIES( rgrl_test_all rgrl rrel testlib vnl_algo vnl vil vul )

ADD_TEST( NAME rgrl_rgrl_util COMMAND $<TARGET_FILE:rgrl_test_all> test_rgrl_util )
ADD_TEST( NAME rgrl_transformation COMMAND $<TARGET_FILE:rgrl_test_all> test_transformation )
ADD_TEST( NAME rgrl_trans_reader COMMAND $<TARGET_FILE:rgrl_test_all> test_trans_reader ${CMAKE_CURRENT_SOURCE_DIR}/affine_xform.txt )
ADD_TEST( NAME rgrl_feature COMMAND $<TARGET_FILE:rgrl_test_all> test_feature )
ADD_TEST( NAME rgrl_feature_region COMMAND $<TARGET_FILE:rgrl_test_all> test_feature_region )
ADD_TEST( NAME rgrl_feature_set COMMAND $<TARGET_FILE:rgrl_test_all> test_feature_set )
ADD_TEST( NAME rgrl_match_set COMMAND $<TARGET_FILE:rgrl_test_all> test_match_set )
ADD_TEST( NAME rgrl_spline COMMAND $<TARGET_FILE:rgrl_test_all> test_spline )
if(0)  # The rgrl_estimtaor test failes too often to be enabled by default:
## https://github.com/vxl/vxl/issues/96
ADD_TEST( NAME rgrl_estimator COMMAND $<TARGET_FILE:rgrl_test_all> test_estimator )
endif()
ADD_TEST( NAME rgrl_convergence COMMAND $<TARGET_FILE:rgrl_test_all> test_convergence )
ADD_TEST( NAME rgrl_scale_est COMMAND $<TARGET_FILE:rgrl_test_all> test_scale_est )
ADD_TEST( NAME rgrl_initializer COMMAND $<TARGET_FILE:rgrl_test_all> test_initializer )
ADD_TEST( NAME rgrl_set_of COMMAND $<TARGET_FILE:rgrl_test_all> test_set_of )
ADD_TEST( NAME rgrl_initializer_ran_sam COMMAND $<TARGET_FILE:rgrl_test_all> test_initializer_ran_sam )
ADD_TEST( NAME rgrl_matcher COMMAND $<TARGET_FILE:rgrl_test_all> test_matcher )

ADD_EXECUTABLE( rgrl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( rgrl_test_include rgrl )
ADD_EXECUTABLE( rgrl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( rgrl_test_template_include rgrl )
