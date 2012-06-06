# mul/msm/tests/CMakeLists.txt

ADD_EXECUTABLE( msm_test_all
  test_driver.cxx
  test_points.cxx
  test_wt_mat_2d.cxx
  test_aligners.cxx
  test_shape_model.cxx
  test_curves.cxx
  test_param_limiters.cxx
)
TARGET_LINK_LIBRARIES( msm_test_all msm testlib )

ADD_TEST( msm_test_points ${EXECUTABLE_OUTPUT_PATH}/msm_test_all test_points)
ADD_TEST( msm_test_wt_mat_2d ${EXECUTABLE_OUTPUT_PATH}/msm_test_all test_wt_mat_2d)
ADD_TEST( msm_test_aligners ${EXECUTABLE_OUTPUT_PATH}/msm_test_all test_aligners)
ADD_TEST( msm_test_shape_model ${EXECUTABLE_OUTPUT_PATH}/msm_test_all test_shape_model)
ADD_TEST( msm_test_curves ${EXECUTABLE_OUTPUT_PATH}/msm_test_all test_curves)
ADD_TEST( msm_test_param_limiters ${EXECUTABLE_OUTPUT_PATH}/msm_test_all test_param_limiters)

ADD_EXECUTABLE( msm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( msm_test_include msm )
