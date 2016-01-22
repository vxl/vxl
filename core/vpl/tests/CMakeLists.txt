# This is core/vpl/tests/CMakeLists.txt

ADD_EXECUTABLE( vpl_test_all
  test_driver.cxx

  test_unistd.cxx
)
TARGET_LINK_LIBRARIES( vpl_test_all vpl testlib vcl )

ADD_TEST( vpl_test_unistd   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vpl_test_all test_unistd ${SITE} )

ADD_EXECUTABLE( vpl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vpl_test_include vpl )
