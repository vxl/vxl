# mul/msm/utils/tests/CMakeLists.txt

add_executable( msm_utils_test_all
  test_driver.cxx
  test_dist_to_curves.cxx
)
target_link_libraries( msm_utils_test_all msm_utils ${VXL_LIB_PREFIX}testlib )

add_test( NAME msm_test_dist_to_curves COMMAND $<TARGET_FILE:msm_utils_test_all> test_dist_to_curves)


add_executable( msm_utils_test_include test_include.cxx )
target_link_libraries( msm_utils_test_include msm_utils)
