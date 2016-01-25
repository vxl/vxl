# This is core/vpl/tests/CMakeLists.txt

add_executable( vpl_test_all
  test_driver.cxx

  test_unistd.cxx
)
target_link_libraries( vpl_test_all vpl testlib vcl )

add_test( NAME vpl_test_unistd COMMAND $<TARGET_FILE:vpl_test_all> test_unistd ${SITE} )

add_executable( vpl_test_include test_include.cxx )
target_link_libraries( vpl_test_include vpl )
