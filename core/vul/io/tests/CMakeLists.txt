# This is core/vul/io/tests/CMakeLists.txt

add_executable( vul_io_test_all
  test_driver.cxx

  test_user_info_io.cxx
)
target_link_libraries( vul_io_test_all  vul_io vul vsl testlib vpl )

add_test( NAME vul_io_test_user_info_io COMMAND $<TARGET_FILE:vul_io_test_all> test_user_info_io )

add_executable( vul_io_test_include test_include.cxx )
target_link_libraries( vul_io_test_include vul_io )
