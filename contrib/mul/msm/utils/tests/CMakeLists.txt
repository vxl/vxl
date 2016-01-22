# mul/msm/utils/tests/CMakeLists.txt

ADD_EXECUTABLE( msm_utils_test_all
  test_driver.cxx
  test_dist_to_curves.cxx
)
TARGET_LINK_LIBRARIES( msm_utils_test_all msm_utils testlib )

ADD_TEST( msm_test_dist_to_curves ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/msm_utils_test_all test_dist_to_curves)


ADD_EXECUTABLE( msm_utils_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( msm_utils_test_include msm_utils)
