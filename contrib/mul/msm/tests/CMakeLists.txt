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

ADD_TEST( msm_test_points ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/msm_test_all test_points)
ADD_TEST( msm_test_wt_mat_2d ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/msm_test_all test_wt_mat_2d)
ADD_TEST( msm_test_aligners ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/msm_test_all test_aligners)
ADD_TEST( msm_test_shape_model ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/msm_test_all test_shape_model)
ADD_TEST( msm_test_curves ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/msm_test_all test_curves)
ADD_TEST( msm_test_param_limiters ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/msm_test_all test_param_limiters)
ADD_TEST( msm_test_cubic_bezier ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/msm_test_all test_cubic_bezier)

ADD_EXECUTABLE( msm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( msm_test_include msm )
