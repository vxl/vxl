# This is mul/ipts/tests/CMakeLists.txt

ADD_EXECUTABLE( ipts_test_all
  test_driver.cxx

  test_scale_space_peaks.cxx
)
TARGET_LINK_LIBRARIES( ipts_test_all ipts testlib )

ADD_TEST( ipts_scale_space_peaks ipts_test_all test_scale_space_peaks )

ADD_EXECUTABLE( ipts_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( ipts_test_include ipts )
