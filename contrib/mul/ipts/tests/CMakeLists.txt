# This is mul/ipts/tests/CMakeLists.txt

ADD_EXECUTABLE( ipts_test_all
  test_driver.cxx

  test_scale_space_peaks.cxx
)
TARGET_LINK_LIBRARIES( ipts_test_all ipts vgl testlib )

ADD_TEST( NAME ipts_test_scale_space_peaks COMMAND $<TARGET_FILE:ipts_test_all> test_scale_space_peaks )

ADD_EXECUTABLE( ipts_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( ipts_test_include ipts )
