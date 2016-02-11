# This is mul/ipts/tests/CMakeLists.txt

add_executable( ipts_test_all
  test_driver.cxx

  test_scale_space_peaks.cxx
)
target_link_libraries( ipts_test_all ipts ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}testlib )

add_test( NAME ipts_test_scale_space_peaks COMMAND $<TARGET_FILE:ipts_test_all> test_scale_space_peaks )

add_executable( ipts_test_include test_include.cxx )
target_link_libraries( ipts_test_include ipts )
