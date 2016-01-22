# mul/msm/tests/CMakeLists.txt

ADD_EXECUTABLE( msm_test_all
  test_driver.cxx
  test_points.cxx
  test_wt_mat_2d.cxx
  test_aligners.cxx
  test_shape_model.cxx
  test_curves.cxx
  test_param_limiters.cxx
  test_cubic_bezier.cxx
)
TARGET_LINK_LIBRARIES( msm_test_all msm testlib )

ADD_TEST( NAME msm_test_points COMMAND $<TARGET_FILE:msm_test_all> test_points)
ADD_TEST( NAME msm_test_wt_mat_2d COMMAND $<TARGET_FILE:msm_test_all> test_wt_mat_2d)
ADD_TEST( NAME msm_test_aligners COMMAND $<TARGET_FILE:msm_test_all> test_aligners)
ADD_TEST( NAME msm_test_shape_model COMMAND $<TARGET_FILE:msm_test_all> test_shape_model)
ADD_TEST( NAME msm_test_curves COMMAND $<TARGET_FILE:msm_test_all> test_curves)
ADD_TEST( NAME msm_test_param_limiters COMMAND $<TARGET_FILE:msm_test_all> test_param_limiters)
ADD_TEST( NAME msm_test_cubic_bezier COMMAND $<TARGET_FILE:msm_test_all> test_cubic_bezier)

ADD_EXECUTABLE( msm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( msm_test_include msm )
