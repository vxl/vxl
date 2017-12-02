# mul/msm/tests/CMakeLists.txt

add_executable( msm_test_all
  test_driver.cxx
  test_points.cxx
  test_wt_mat_2d.cxx
  test_aligners.cxx
  test_shape_model.cxx
  test_curves.cxx
  test_param_limiters.cxx
  test_cubic_bezier.cxx
)
target_link_libraries( msm_test_all msm ${VXL_LIB_PREFIX}testlib )

add_test( NAME msm_test_points COMMAND $<TARGET_FILE:msm_test_all> test_points)
add_test( NAME msm_test_wt_mat_2d COMMAND $<TARGET_FILE:msm_test_all> test_wt_mat_2d)
add_test( NAME msm_test_aligners COMMAND $<TARGET_FILE:msm_test_all> test_aligners)
add_test( NAME msm_test_shape_model COMMAND $<TARGET_FILE:msm_test_all> test_shape_model)
add_test( NAME msm_test_curves COMMAND $<TARGET_FILE:msm_test_all> test_curves)
add_test( NAME msm_test_param_limiters COMMAND $<TARGET_FILE:msm_test_all> test_param_limiters)
add_test( NAME msm_test_cubic_bezier COMMAND $<TARGET_FILE:msm_test_all> test_cubic_bezier)

add_executable( msm_test_include test_include.cxx )
target_link_libraries( msm_test_include msm )
