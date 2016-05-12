include_directories( ${VXL_MUL_INCLUDE_DIR} )

add_executable( rgrl_test_all
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
target_link_libraries( rgrl_test_all rgrl rrel ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul )

add_test( NAME rgrl_rgrl_util COMMAND $<TARGET_FILE:rgrl_test_all> test_rgrl_util )
add_test( NAME rgrl_transformation COMMAND $<TARGET_FILE:rgrl_test_all> test_transformation )
add_test( NAME rgrl_trans_reader COMMAND $<TARGET_FILE:rgrl_test_all> test_trans_reader ${CMAKE_CURRENT_SOURCE_DIR}/affine_xform.txt )
add_test( NAME rgrl_feature COMMAND $<TARGET_FILE:rgrl_test_all> test_feature )
add_test( NAME rgrl_feature_region COMMAND $<TARGET_FILE:rgrl_test_all> test_feature_region )
add_test( NAME rgrl_feature_set COMMAND $<TARGET_FILE:rgrl_test_all> test_feature_set )
add_test( NAME rgrl_match_set COMMAND $<TARGET_FILE:rgrl_test_all> test_match_set )
add_test( NAME rgrl_spline COMMAND $<TARGET_FILE:rgrl_test_all> test_spline )
if(1)  
# Gary in the process of fixing this issue. Need dashboard to assist
# The rgrl_estimtaor test failes too often to be enabled by default:
## https://github.com/vxl/vxl/issues/96
add_test( NAME rgrl_estimator COMMAND $<TARGET_FILE:rgrl_test_all> test_estimator )
endif()
add_test( NAME rgrl_convergence COMMAND $<TARGET_FILE:rgrl_test_all> test_convergence )
add_test( NAME rgrl_scale_est COMMAND $<TARGET_FILE:rgrl_test_all> test_scale_est )
add_test( NAME rgrl_initializer COMMAND $<TARGET_FILE:rgrl_test_all> test_initializer )
add_test( NAME rgrl_set_of COMMAND $<TARGET_FILE:rgrl_test_all> test_set_of )
add_test( NAME rgrl_initializer_ran_sam COMMAND $<TARGET_FILE:rgrl_test_all> test_initializer_ran_sam )
add_test( NAME rgrl_matcher COMMAND $<TARGET_FILE:rgrl_test_all> test_matcher )

add_executable( rgrl_test_include test_include.cxx )
target_link_libraries( rgrl_test_include rgrl )
add_executable( rgrl_test_template_include test_template_include.cxx )
target_link_libraries( rgrl_test_template_include rgrl )
