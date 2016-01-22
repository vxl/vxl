# mul/msm/utils/tests/CMakeLists.txt

ADD_EXECUTABLE( msm_utils_test_all
  test_driver.cxx
  test_dist_to_curves.cxx
)
TARGET_LINK_LIBRARIES( msm_utils_test_all msm_utils testlib )

ADD_TEST( NAME msm_test_dist_to_curves COMMAND $<TARGET_FILE:msm_utils_test_all> test_dist_to_curves)


ADD_EXECUTABLE( msm_utils_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( msm_utils_test_include msm_utils)
