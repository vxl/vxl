# mul/msm/tests/CMakeLists.txt

ADD_EXECUTABLE( msm_test_all
  test_driver.cxx
  test_points.cxx
  )
TARGET_LINK_LIBRARIES( msm_test_all msm testlib )

ADD_TEST( msm_test_points ${EXECUTABLE_OUTPUT_PATH}/msm_test_all test_points)

# ADD_EXECUTABLE( msm_test_include test_include.cxx )
# TARGET_LINK_LIBRARIES( msm_test_include msm )
