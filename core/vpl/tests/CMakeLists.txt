# This is core/vpl/tests/CMakeLists.txt

ADD_EXECUTABLE( vpl_test_all
  test_driver.cxx

  test_unistd.cxx
)
TARGET_LINK_LIBRARIES( vpl_test_all vpl testlib vcl )

ADD_TEST( NAME vpl_test_unistd COMMAND $<TARGET_FILE:vpl_test_all> test_unistd ${SITE} )

ADD_EXECUTABLE( vpl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vpl_test_include vpl )
