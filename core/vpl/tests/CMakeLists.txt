# This is core/vpl/tests/CMakeLists.txt

ADD_EXECUTABLE( vpl_test_all
  test_driver.cxx

  test_unistd.cxx
)
TARGET_LINK_LIBRARIES( vpl_test_all vpl testlib vcl )

ADD_TEST( vpl_test_unistd   ${EXECUTABLE_OUTPUT_PATH}/vpl_test_all test_unistd )

ADD_EXECUTABLE( vpl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vpl_test_include vpl )
